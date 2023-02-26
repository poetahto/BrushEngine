call vcvarsall amd64

call msbuild -maxCpuCount -p:Configuration=Debug ./src
call robocopy ./assets ./bin/x64/Debug/assets /MIR

call explorer bin\x64\Debug