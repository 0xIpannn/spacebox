#include "physics/body.h"
#include "physics/constants.h"
#include "utils/bodyVector.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

#define DEG_TO_RAD (PI / 180.0)

// Approximate heliocentric orbital elements for epoch J2000.0.
//
// Distances are converted from AU to SpaceBox distance units. The elements
// describe an initial state, not a continuously updated ephemeris; after the
// simulation starts, the N-body solver determines the subsequent motion.
typedef struct {
  const char *name;
  double semiMajorAxisAU;
  double eccentricity;
  double inclinationDeg;
  double meanLongitudeDeg;
  double longitudeOfPerihelionDeg;
  double longitudeOfAscendingNodeDeg;
  float mass; // solar masses
  float visualRadius;
  Color color;
  bool hasAtmosphere;
  float atmosphereThickness;
  Color atmosphereColor;
} PlanetData;

static double normalize_angle(double angle) {
  angle = fmod(angle, 2.0 * PI);
  return angle < 0.0 ? angle + 2.0 * PI : angle;
}

// Solves Kepler's equation: M = E - e*sin(E).
static double eccentric_anomaly(double meanAnomaly, double eccentricity) {
  double eccentricAnomaly = meanAnomaly;

  for (int iteration = 0; iteration < 12; iteration++) {
    double correction =
        (eccentricAnomaly - eccentricity * sin(eccentricAnomaly) -
         meanAnomaly) /
        (1.0 - eccentricity * cos(eccentricAnomaly));
    eccentricAnomaly -= correction;

    if (fabs(correction) < 1.0e-12)
      break;
  }

  return eccentricAnomaly;
}

static Vector3 rotate_from_orbital_plane(Vector3 vector, double ascendingNode,
                                         double inclination,
                                         double argumentOfPeriapsis) {
  double cosNode = cos(ascendingNode);
  double sinNode = sin(ascendingNode);
  double cosInclination = cos(inclination);
  double sinInclination = sin(inclination);
  double cosPeriapsis = cos(argumentOfPeriapsis);
  double sinPeriapsis = sin(argumentOfPeriapsis);

  // Rotation: Rz(ascendingNode) * Rx(inclination) * Rz(argumentOfPeriapsis)
  double x = (cosNode * cosPeriapsis -
              sinNode * sinPeriapsis * cosInclination) * vector.x +
             (-cosNode * sinPeriapsis -
              sinNode * cosPeriapsis * cosInclination) * vector.y;
  double y = (sinNode * cosPeriapsis +
              cosNode * sinPeriapsis * cosInclination) * vector.x +
             (-sinNode * sinPeriapsis +
              cosNode * cosPeriapsis * cosInclination) * vector.y;
  double z = (sinPeriapsis * sinInclination) * vector.x +
             (cosPeriapsis * sinInclination) * vector.y;

  // Orbital elements use X/Y as the reference plane. Raylib uses X/Z as
  // the horizontal plane, so map the ecliptic normal (Z) to raylib Y.
  return (Vector3){(float)x, (float)z, (float)y};
}

static void orbital_state(const PlanetData *data, Vector3 *position,
                          Vector3 *velocity) {
  const double auInDistanceUnits = 149597870.7 / DISTANCE_UNIT;
  const double a = data->semiMajorAxisAU * auInDistanceUnits;
  const double e = data->eccentricity;
  const double inclination = data->inclinationDeg * DEG_TO_RAD;
  const double ascendingNode =
      data->longitudeOfAscendingNodeDeg * DEG_TO_RAD;
  const double longitudeOfPerihelion =
      data->longitudeOfPerihelionDeg * DEG_TO_RAD;
  const double argumentOfPeriapsis =
      longitudeOfPerihelion - ascendingNode;

  // The mean longitude and longitude of perihelion provide the J2000 mean
  // anomaly. Convert it to true anomaly through Kepler's equation.
  const double meanLongitude = data->meanLongitudeDeg * DEG_TO_RAD;
  const double meanAnomaly =
      normalize_angle(meanLongitude - longitudeOfPerihelion);
  const double eccentricAnomaly = eccentric_anomaly(meanAnomaly, e);
  const double trueAnomaly =
      2.0 * atan2(sqrt(1.0 + e) * sin(eccentricAnomaly / 2.0),
                  sqrt(1.0 - e) * cos(eccentricAnomaly / 2.0));

  const double radius = a * (1.0 - e * cos(eccentricAnomaly));
  const double parameter = a * (1.0 - e * e);
  const double gravitationalParameter = G * (1.0 + data->mass);
  const double velocityScale = sqrt(gravitationalParameter / parameter);

  Vector3 positionInOrbitalPlane =
      {(float)(radius * cos(trueAnomaly)),
       (float)(radius * sin(trueAnomaly)), 0.0f};
  Vector3 velocityInOrbitalPlane =
      {(float)(-velocityScale * sin(trueAnomaly)),
       (float)(velocityScale * (e + cos(trueAnomaly))), 0.0f};

  *position = rotate_from_orbital_plane(
      positionInOrbitalPlane, ascendingNode, inclination, argumentOfPeriapsis);
  *velocity = rotate_from_orbital_plane(
      velocityInOrbitalPlane, ascendingNode, inclination, argumentOfPeriapsis);
}

// Put the initial system in its center-of-mass frame. This allows the Sun to
// move naturally instead of forcing it to remain at the origin.
static void make_barycentric(BodyVector *bodies) {
  float totalMass = 0.0f;
  Vector3 centerOfMass = Vector3Zero();
  Vector3 centerOfMassVelocity = Vector3Zero();

  for (size_t i = 0; i < bodies->count; i++) {
    Body *body = &bodies->data[i];
    if (!body->isActive || body->mass <= 0.0f)
      continue;

    totalMass += body->mass;
    centerOfMass = Vector3Add(
        centerOfMass, Vector3Scale(body->position, body->mass));
    centerOfMassVelocity = Vector3Add(
        centerOfMassVelocity, Vector3Scale(body->velocity, body->mass));
  }

  if (totalMass <= 0.0f)
    return;

  centerOfMass = Vector3Scale(centerOfMass, 1.0f / totalMass);
  centerOfMassVelocity =
      Vector3Scale(centerOfMassVelocity, 1.0f / totalMass);

  for (size_t i = 0; i < bodies->count; i++) {
    bodies->data[i].position =
        Vector3Subtract(bodies->data[i].position, centerOfMass);
    bodies->data[i].velocity =
        Vector3Subtract(bodies->data[i].velocity, centerOfMassVelocity);
  }
}

void solar_system_init(BodyVector *bodies) {
  // Masses are solar masses. Rendering radii are intentionally exaggerated
  // because physically scaled planet radii would be sub-pixel at this scale.
  Body sun = body_create_star("Sun", Vector3Zero(), Vector3Zero(), 1.0f, 8.0f,
                              GOLD, 1.0f);
  vector_add(bodies, sun);

  static const PlanetData planets[] = {
      // name, a (AU), e, inclination, mean longitude, longitude of
      // perihelion, ascending node, mass (M_sun), visual radius, color,
      // atmosphere, thickness, atmosphere color
      {"Mercury", 0.38709927, 0.20563593, 7.00497902, 252.25032350,
       77.45779628, 48.33076593, 1.660e-7f, 0.35f, GRAY, false, 0.0f, WHITE},
      {"Venus", 0.72333566, 0.00677672, 3.39467605, 181.97909950,
       131.60246718, 76.67984255, 2.447e-6f, 0.55f, ORANGE, true, 0.18f,
       (Color){230, 190, 130, 100}},
      {"Earth", 1.00000261, 0.01671123, -0.00001531, 100.46457166,
       102.93768193, 0.0, 3.003e-6f, 0.65f, BLUE, true, 0.10f,
       (Color){80, 170, 255, 90}},
      {"Mars", 1.52371034, 0.09339410, 1.84969142, -4.55343205,
       -23.94362959, 49.55953891, 3.227e-7f, 0.48f, RED, true, 0.04f,
       (Color){210, 120, 100, 55}},
      {"Ceres", 2.76718174, 0.07584220, 10.59406700, 239.52900000,
       73.59770000, 80.30550000, 4.73e-10f, 0.22f, DARKGRAY, false, 0.0f,
       WHITE},
      {"Jupiter", 5.20288700, 0.04838624, 1.30439695, 34.39644051,
       14.72847983, 100.47390909, 9.543e-4f, 2.25f, BEIGE, true, 0.20f,
       (Color){220, 190, 140, 45}},
      {"Saturn", 9.53667594, 0.05386179, 2.48599187, 49.95424423,
       92.59887831, 113.66242448, 2.857e-4f, 1.85f, LIGHTGRAY, true, 0.18f,
       (Color){210, 200, 170, 40}},
      {"Uranus", 19.18916464, 0.04725744, 0.77263783, 313.23810451,
       170.95427630, 74.01692503, 4.366e-5f, 1.35f, SKYBLUE, true, 0.15f,
       (Color){130, 220, 235, 40}},
      {"Neptune", 30.06992276, 0.00859048, 1.77004347, -55.12002969,
       44.96476227, 131.78422574, 5.151e-5f, 1.30f, DARKBLUE, true, 0.15f,
       (Color){80, 120, 240, 40}},
      {"Pluto", 39.48211675, 0.24882730, 17.14001206, 238.92903833,
       224.06891629, 110.30393684, 6.55e-9f, 0.25f, BROWN, true, 0.03f,
       (Color){180, 170, 210, 30}},
  };

  size_t count = sizeof(planets) / sizeof(planets[0]);
  for (size_t i = 0; i < count; i++) {
    Vector3 position;
    Vector3 velocity;
    orbital_state(&planets[i], &position, &velocity);

    Body planet = body_create_planet(
        planets[i].name, position, velocity, planets[i].mass,
        planets[i].visualRadius, planets[i].color, planets[i].hasAtmosphere,
        planets[i].atmosphereThickness, planets[i].atmosphereColor);
    vector_add(bodies, planet);
  }

  make_barycentric(bodies);
}
