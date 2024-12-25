type GLFWmonitor
type GLFWwindow

extern glfwInit()
extern glfwTerminate()
extern glfwCreateWindow(width: i32, height: i32, title: i8[], monitor: GLFWmonitor[], share: GLFWwindow[]): GLFWwindow[]
extern glfwDestroyWindow(window: GLFWwindow[])
extern glfwMakeContextCurrent(window: GLFWwindow[])
extern glfwSwapInterval(interval: i32)
extern glfwWindowShouldClose(window: GLFWwindow[]): i32
extern glfwPollEvents()
extern glfwSwapBuffers(window: GLFWwindow[])

glfwInit()
let window = glfwCreateWindow(800, 600, "Test", 0, 0)
glfwMakeContextCurrent(window)
glfwSwapInterval(1)

for (; glfwWindowShouldClose(window) != 1; ) {
    glfwPollEvents()
    glfwSwapBuffers(window)
}

glfwDestroyWindow(window)
glfwTerminate()
