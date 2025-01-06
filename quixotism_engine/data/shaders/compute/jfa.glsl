#version 430

layout(local_size_x = 1, local_size_y = 1) in;

// Bind the depth-stencil texture as an image
layout(binding = 0, rg32f) readonly uniform image2D jfa_in_texture;
layout(binding = 1, rg32f) writeonly uniform image2D jfa_out_texture;

uniform int step_size;

// Function to compute squared distance between two points
float distanceSquared(vec2 p1, vec2 p2) {
  vec2 diff = vec2(p1 - p2);
  return dot(diff, diff);
}

void main() {
  ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
  ivec2 imgSize = ivec2(800, 600);

  // Read current pixel's data from input texture
  vec2 closestSeedData = imageLoad(jfa_in_texture, coords).xy;

  // Initialize minimum distance with a large value
  float minDistSquared = 9999999.9999;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue;  // Skip the current pixel

      ivec2 offset = ivec2(dx, dy) * step_size;
      ivec2 neighborCoords = coords + offset;

      // Check if the neighbor coordinates are within bounds
      if (neighborCoords.x >= 0 && neighborCoords.x < imgSize.x &&
          neighborCoords.y >= 0 && neighborCoords.y < imgSize.y) {
        // Read the neighbor pixel's data
        vec2 neighborData = imageLoad(jfa_in_texture, neighborCoords).xy;

        if (neighborData.x < 0) continue;

        // Compute the squared distance from the current pixel to the neighbor's
        // seed point
        float distSquared =
            distanceSquared((vec2(coords) / vec2(imgSize)), neighborData);

        // Update the minimum distance and closest seed data if necessary
        if (distSquared < minDistSquared) {
          minDistSquared = distSquared;
          closestSeedData = neighborData;
        }
      }
    }
  }

  // Write the output value to the output texture
  imageStore(jfa_out_texture, coords, vec4(closestSeedData, 0.0, 1.0));
}