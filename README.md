# EasyBot86

## Build in GitHub Actions

This project can be built in the cloud so you do not need local disk space for `grpc` and `protobuf`.

### What is included

- Workflow: `.github/workflows/build-easybot-dll.yml`
- Target: `EasyBotDLL`
- Output artifact: `easybot-dll-win32`
- DLL inside artifact: `EasyBot.dll`

### How to use it

1. Create a new GitHub repository and upload this folder as the repository contents.
2. Open the repository in GitHub.
3. Go to `Actions`.
4. Run the workflow `Build EasyBot DLL`.
5. After the job finishes, download the artifact `easybot-dll-win32`.

### Why this workflow uses `B:`

The bundled `vcpkg` + `grpc` tree gets very deep paths on Windows. The workflow maps the checkout to `B:` before running CMake so the cloud build avoids the same path-length problems seen locally.
