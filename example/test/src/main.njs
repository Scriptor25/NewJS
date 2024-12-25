import glfw from "./glfw3.njs"
import   gl from    "./gl.njs"

glfw.init()
let window = glfw.createWindow(800, 600, "Test", 0, 0)
glfw.makeContextCurrent(window)
glfw.swapInterval(1)
glfw.setKeyCallback(window, ?(window: GLFWwindow[], key: i32, scancode: i32, action: i32, mods: i32) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfw.setWindowShouldClose(window, GLFW_TRUE)
})

gl.clearColor(0.2, 0.3, 1.0, 1.0)

for (; glfw.windowShouldClose(window) != GLFW_TRUE; ) {
    glfw.pollEvents()

    let width: i32
    let height: i32
    glfw.getFramebufferSize(window, &width, &height)

    gl.viewport(0, 0, width, height)
    gl.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    glfw.swapBuffers(window)
}

glfw.destroyWindow(window)
glfw.terminate()
