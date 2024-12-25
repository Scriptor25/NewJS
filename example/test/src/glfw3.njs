type GLFWmonitor
type GLFWwindow

#GLFW_TRUE       "  1:i32"
#GLFW_KEY_ESCAPE "256:i32"
#GLFW_RELEASE    "  0:i32"

extern glfwInit()
extern glfwTerminate()
extern glfwCreateWindow(width: i32, height: i32, title: i8[], monitor: GLFWmonitor[], share: GLFWwindow[]): GLFWwindow[]
extern glfwDestroyWindow(window: GLFWwindow[])
extern glfwMakeContextCurrent(window: GLFWwindow[])
extern glfwSwapInterval(interval: i32)
extern glfwWindowShouldClose(window: GLFWwindow[]): i32
extern glfwPollEvents()
extern glfwSwapBuffers(window: GLFWwindow[])
extern glfwGetFramebufferSize(window: GLFWwindow[], width: i32[], height: i32[])
extern glfwSetKeyCallback(window: GLFWwindow[], callback: (GLFWwindow[], i32, i32, i32, i32))
extern glfwSetWindowShouldClose(window: GLFWwindow[], closed: i32)

function init() { glfwInit() }
function terminate() { glfwTerminate() }
function createWindow(width: i32, height: i32, title: i8[], monitor: GLFWmonitor[], share: GLFWwindow[]): GLFWwindow[] { return glfwCreateWindow(width, height, title, monitor, share) }
function destroyWindow(window: GLFWwindow[]) { glfwDestroyWindow(window) }
function makeContextCurrent(window: GLFWwindow[]) { glfwMakeContextCurrent(window) }
function swapInterval(interval: i32) { glfwSwapInterval(interval) }
function windowShouldClose(window: GLFWwindow[]): i32 { return glfwWindowShouldClose(window) }
function pollEvents() { glfwPollEvents() }
function swapBuffers(window: GLFWwindow[]) { glfwSwapBuffers(window) }
function getFramebufferSize(window: GLFWwindow[], width: i32[], height: i32[]) { glfwGetFramebufferSize(window, width, height) }
function setKeyCallback(window: GLFWwindow[], callback: (GLFWwindow[], i32, i32, i32, i32)) { glfwSetKeyCallback(window, callback) }
function setWindowShouldClose(window: GLFWwindow[], closed: i32) { glfwSetWindowShouldClose(window, closed) }
