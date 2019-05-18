//#define LAUNCH_CLI

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <Windows.h>
#include <Shlwapi.h>
#include <string>
#include <filesystem>

std::filesystem::path getRootFolder()
{
    std::vector<WCHAR> res(MAX_PATH);

    DWORD length = GetModuleFileNameW(NULL, res.data(), res.size());
    PathRemoveFileSpecW(res.data());
    return res.data();
}

bool findNode(std::filesystem::path& NodePath)
{
    const std::wstring NodeName = L"node.exe";
    std::vector<WCHAR> res(MAX_PATH);
    std::copy(NodeName.begin(), NodeName.end(), res.begin());
    res[NodeName.size()] = 0;

    if (!PathFindOnPathW(res.data(), NULL))
        return false;

    NodePath = res.data();
    return true;
}

bool launch(const std::filesystem::path& ExecutableName, const std::wstring& CommandLine)
{
    STARTUPINFOW StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    PROCESS_INFORMATION ProcessInformation;
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

    if (!CreateProcessW(ExecutableName.wstring().c_str(), (LPWSTR)CommandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInformation))
        return false;

    CloseHandle(ProcessInformation.hProcess);
    CloseHandle(ProcessInformation.hThread);
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    const auto RootFolder = getRootFolder();

#ifdef LAUNCH_CLI
    std::filesystem::path NodeExe;
    if (!findNode(NodeExe))
    {
        MessageBoxW(NULL, L"Node.JS is not installed!\nPlease go to https://discord.gg/dUNDDtw and follow the installation guide.", L"Error", 0);
        return 1;
    }

    if (!launch(NodeExe, L"--use-strict ./bin/index-cli.js"))
    {
        MessageBoxW(NULL, L"Unable to launch TERA Toolbox!\nPlease go to https://discord.gg/dUNDDtw and ask for help.", L"Error", 0);
        return 1;
    }
#else
    const auto ElectronExe = RootFolder / L"node_modules/electron/dist/electron.exe";

    if (std::filesystem::exists(ElectronExe))
    {
        if (!launch(ElectronExe, L"--high-dpi-support=1 --force-device-scale-factor=1 --js-flags=\"--use-strict\" ./bin/index-gui.js"))
        {
            MessageBoxW(NULL, L"Unable to launch TERA Toolbox!\nPlease go to https://discord.gg/dUNDDtw and ask for help.", L"Error", 0);
            return 1;
        }
    }
    else
    {
        std::filesystem::path NodeExe;
        if (!findNode(NodeExe))
        {
            MessageBoxW(NULL, L"Node.JS is not installed!\nPlease go to https://discord.gg/dUNDDtw and follow the installation guide.", L"Error", 0);
            return 1;
        }

        if (!launch(NodeExe, L"--use-strict ./bin/install-electron.js"))
        {
            MessageBoxW(NULL, L"Unable to launch TERA Toolbox!\nPlease go to https://discord.gg/dUNDDtw and ask for help.", L"Error", 0);
            return 1;
        }
    }
#endif

    return 0;
}
