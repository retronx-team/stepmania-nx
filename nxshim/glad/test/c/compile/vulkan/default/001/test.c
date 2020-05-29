/*
 * Full Vulkan
 *
 * GLAD: $GLAD --out-path=$tmp --api="vulkan" c --loader
 * COMPILE: $GCC $test -o $tmp/test -I$tmp/include $tmp/src/vulkan.c -ldl
 * RUN: $tmp/test
 */

#include <glad/vulkan.h>

int main(void) {
    (void) gladLoaderLoadVulkan(NULL, NULL, NULL);
    return 0;
}
