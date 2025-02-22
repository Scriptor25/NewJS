type GLFWmonitor = void
type GLFWwindow  = void

#GLFW_TRUE       "  1:i32"
#GLFW_KEY_ESCAPE "256:i32"
#GLFW_RELEASE    "  0:i32"

extern function glfwInit(): i32
extern function glfwTerminate()
extern function glfwCreateWindow(width: i32, height: i32, title: i8[], monitor: GLFWmonitor[], share: GLFWwindow[]): GLFWwindow[]
extern function glfwDestroyWindow(window: GLFWwindow[])
extern function glfwMakeContextCurrent(window: GLFWwindow[])
extern function glfwSwapInterval(interval: i32)
extern function glfwWindowShouldClose(window: GLFWwindow[]): i32
extern function glfwPollEvents()
extern function glfwSwapBuffers(window: GLFWwindow[])
extern function glfwGetFramebufferSize(window: GLFWwindow[], width: i32[], height: i32[])
extern function glfwSetKeyCallback(window: GLFWwindow[], callback: (GLFWwindow[], i32, i32, i32, i32))
extern function glfwSetWindowShouldClose(window: GLFWwindow[], closed: i32)
extern function glfwGetError(description: i8[][]): i32
extern function glfwSetErrorCallback(callback: (i32, i32)): (i32, i32)

export function init(): i32 { return glfwInit() }
export function terminate() { glfwTerminate() }
export function createWindow(width: i32, height: i32, title: i8[], monitor: GLFWmonitor[], share: GLFWwindow[]): GLFWwindow[] { return glfwCreateWindow(width, height, title, monitor, share) }
export function destroyWindow(window: GLFWwindow[]) { glfwDestroyWindow(window) }
export function makeContextCurrent(window: GLFWwindow[]) { glfwMakeContextCurrent(window) }
export function swapInterval(interval: i32) { glfwSwapInterval(interval) }
export function windowShouldClose(window: GLFWwindow[]): i32 { return glfwWindowShouldClose(window) }
export function pollEvents() { glfwPollEvents() }
export function swapBuffers(window: GLFWwindow[]) { glfwSwapBuffers(window) }
export function getFramebufferSize(window: GLFWwindow[], width: i32[], height: i32[]) { glfwGetFramebufferSize(window, width, height) }
export function setKeyCallback(window: GLFWwindow[], callback: (GLFWwindow[], i32, i32, i32, i32)) { glfwSetKeyCallback(window, callback) }
export function setWindowShouldClose(window: GLFWwindow[], closed: i32) { glfwSetWindowShouldClose(window, closed) }
export function getError(description: i8[][]): i32 { return glfwGetError(description) }
export function setErrorCallback(callback: (i32, i32)): (i32, i32) { return glfwSetErrorCallback(callback) }
