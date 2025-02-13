type GLFWglproc = ()

#GL_COLOR_BUFFER_BIT "0x00004000:u32"
#GL_DEPTH_BUFFER_BIT "0x00000100:u32"

extern function glfwGetProcAddress(procname: i8[]): GLFWglproc
function getProcAddress(procname: i8[]): GLFWglproc { return glfwGetProcAddress(procname) }
#getProc(NAME) "(getProcAddress(\"#NAME\") as PFN##NAMEPROC)"

type PFNGLCLEARCOLORPROC = (f32, f32, f32, f32)
type PFNGLVIEWPORTPROC = (i32, i32, i32, i32)
type PFNGLCLEARPROC = (i32)

function clearColor(r: f32, g: f32, b: f32, a: f32) { getProc(glClearColor)(r, g, b, a) }
function viewport(x: i32, y: i32, width: i32, height: i32) { getProc(glViewport)(x, y, width, height) }
function clear(bits: i32) { getProc(glClear)(bits) }
