#version 460 core

in vec2 vUV;
out vec4 FragColor;

// Uniforms
uniform sampler2D uCloudTex;
uniform vec3 uViewDir;
uniform float uAspect;
uniform float uTanHalfFov;
uniform float uSunAngle;
uniform int uSamples;

// Constants
const float PI = 3.14159265359;
const float Hr = 8000;
const float Hm = 1200;
const float Ho = 8000;

const vec3 rayleigh = vec3(5.8, 13.5, 33.1) * 1e-6;
const vec3 mie = vec3(21, 21, 21) * 1e-6;
const vec3 ozone = vec3(3.426, 8.298, 0.356) * 0.06 * 1e-5;
const float radiusEarth = 6360e3;
const float radiusAtmo = 6420e3;
const float ZenithH = radiusAtmo - radiusEarth;
const vec3 origin_view = vec3(0, 0, radiusEarth + 1);
const float originH = origin_view.z - radiusEarth;

const float radius_size = radians(0.53 / 2.0);
const float sun_solid_angle = 2 * PI * (1 - cos(radius_size));

const vec3 TrZenith =
    exp(-(rayleigh * Hr * (exp(-originH / Hr) - exp(-ZenithH / Hr)) +
          mie * Hm * (exp(-originH / Hm) - exp(-ZenithH / Hm)) +
          ozone * Ho * (exp(-originH / Ho) - exp(-ZenithH / Ho))));

vec3 illuminanceGround = 120000 * (vec3(1.0, 1.0, 1.0));
vec3 L_outerspace = (illuminanceGround / sun_solid_angle) / TrZenith;

// Helpers
float intersectRaySphereFromInside(const vec3 rayOrigin, const vec3 rayDir,
                                   float radius) {
  float b = dot(rayOrigin, rayDir);
  float c = dot(rayOrigin, rayOrigin) - radius * radius;
  float discriminant = b * b - c;
  float t = -b + sqrt(discriminant);
  return t;
}

float rayleigh_phase(vec3 view_dir, vec3 sun_dir) {
  const float mu = dot(view_dir, sun_dir);
  return (3.0 / (16.0 * PI)) * (1.f + mu * mu);
}

float mie_phase(vec3 view_dir, vec3 sun_dir) {
  float mu = dot(view_dir, sun_dir);
  float g = 0.76;
  float denom = 1.0 + g * g - 2.0 * g * mu;
  return (1.0 - g * g) / (4.0 * PI * pow(denom, 1.5));
}

vec3 sigma_s_rayleigh(vec3 position) {
  const float h = length(position) - radiusEarth;
  return rayleigh * exp(-h / Hr);
}

vec3 sigma_s_mie(vec3 position) {
  const float h = length(position) - radiusEarth;
  return mie * exp(-h / Hm);
}

vec3 sigma_a_ozone(vec3 position) {
  const float h = length(position) - radiusEarth;
  return ozone * exp(-h / Ho);
}

vec3 sigma_t(vec3 position) {
  return sigma_s_rayleigh(position) + 1.11 * sigma_s_mie(position) +
         sigma_a_ozone(position);
}

vec3 integrate_sigma_t(vec3 from, vec3 to) {
  const vec3 ds = (to - from) / float(uSamples);
  vec3 accumulation = vec3(0, 0, 0);

  for (int i = 0; i < uSamples; ++i) {
    const vec3 s = from + (i + 0.5) * ds;
    accumulation += sigma_t(s);
  }

  return accumulation * length(ds);
}

vec3 transmittance(vec3 from, vec3 to) {
  const vec3 integral = integrate_sigma_t(from, to);
  return exp(-integral);
}

vec3 j(vec3 position, vec3 view_dir, vec3 sun_dir) {
  float sunDist = intersectRaySphereFromInside(position, sun_dir, radiusAtmo);

  vec3 extinction = sigma_t(position);

  vec3 trToSun = exp(-extinction * sunDist);
  const vec3 rayleigh_diffusion =
      sigma_s_rayleigh(position) * rayleigh_phase(view_dir, sun_dir);
  const vec3 mie_diffusion =
      sigma_s_mie(position) * mie_phase(view_dir, sun_dir);

  return L_outerspace * trToSun * sun_solid_angle *
         (rayleigh_diffusion + mie_diffusion);
}

vec3 compute_luminance(vec3 out_atmosphere, vec3 sun_dir) {
  const vec3 ds = (out_atmosphere - origin_view) / uSamples;
  const vec3 direction = normalize(ds);
  vec3 acc = vec3(0.0);

  for (int i = 0; i < uSamples; ++i) {
    const vec3 s = origin_view + (i + 0.5) * ds;
    acc += transmittance(origin_view, s) * j(s, direction, sun_dir);
  }

  return acc * length(ds);
}

vec3 direct_light_from_sun(vec3 direction, vec3 out_atmosphere, vec3 sun_dir) {
  float cos_theta = dot(direction, sun_dir);

  const float angle = acos(cos_theta);
  float disk = 1.0 - smoothstep(radius_size * 0.95, radius_size * 1.05, angle);

  return disk * L_outerspace * transmittance(origin_view, out_atmosphere);
}

vec3 ACESFilm(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 toneMap(vec3 color) {
  float exposure = 0.00015;
  color *= exposure;

  color = ACESFilm(color);

  // gamma for SDR
  color = pow(color, vec3(1.0 / 2.2));

  return color;
}

void main() {
  // convert UV to screen space with FOV
  vec2 screen = vUV * 2.0 - 1.0;
  screen.x *= uAspect * uTanHalfFov;
  screen.y *= uTanHalfFov;

  // camera basis
  vec3 forward = normalize(uViewDir);
  vec3 worldUp = vec3(0, 0, 1);
  vec3 right = normalize(cross(forward, worldUp));
  vec3 up = normalize(cross(right, forward));

  // ray direction
  vec3 rayDir = normalize(forward + right * screen.x + up * screen.y);

  // Render atmosphere
  const vec3 sun_dir = normalize(vec3(0, cos(radians(uSunAngle)), sin(radians(uSunAngle))));

  const float distance_out =
      intersectRaySphereFromInside(origin_view, rayDir, radiusAtmo);
  const vec3 view_out = origin_view + rayDir * distance_out;

  vec3 luminance = compute_luminance(view_out, sun_dir);
  luminance += direct_light_from_sun(rayDir, view_out, sun_dir);

  vec3 color = toneMap(luminance);

  // output
  FragColor = vec4(color, 1);
}