# Getting Started
This server uses socket functions that will only compile on Linux and BSD variants (potentially compiles on other UNIX-like operating systems).This server is not compliant with the HTTP Protocol as it does not implement all of the required headers, security features, or any caching whatsoever. It is for educational purposes only. It currently implements from HTTP/1.1:

- Status Line
- Date Header
- Content-Type Header
- Content-Length Header

And is compatible with these filetypes:

- text/html
- text/css
- application/javascript
- image/png
- image/jpeg
- image/gif
- image/x-icon

## Step 1:
Export the root directory of the website you want to serve as an environment variable:
`export WEBSITE_ROOT="{website_dir}"`.
Save this in your ~/.bashrc or an equivalent shell config file to make this permanent

## Step 2:
Configure CMake to build your executable in the 'build' directory:
`cmake -S "${workspaceFolder}" -B "${workspaceFolder}/build" -DCMAKE_BUILD_TYPE=Debug`
Optionally, you can change the name of the executable in CMakeLists.txt as well as some other customization options

## Step 3:
Build the executable:
`cmake --build ${workspaceFolder}/build`

## Step 4:
Navigate to the build directory and run the executable:
`./BugHTTPServer`

Your website should now be accessible at 127.0.0.1:8080