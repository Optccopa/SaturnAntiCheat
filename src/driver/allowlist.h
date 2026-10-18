#define R(s) RTL_CONSTANT_STRING(s)

static const UNICODE_STRING kCs2Rel   = R(L"\\bin\\win64\\cs2.exe");
static const UNICODE_STRING kNtdllRel = R(L"\\ntdll.dll");
static const UNICODE_STRING kSteamRel = R(L"\\steam.exe");

static UNICODE_STRING g_GameRoot;   // ...\Counter-Strike Global Offensive\game
static UNICODE_STRING g_Sys32Root;  // ...\Windows\System32
static UNICODE_STRING g_SteamRoot;  // ...\Steam

static const UNICODE_STRING g_GameRel[] = {
    R(L"\\bin\\win64\\cs2.exe"),
    R(L"\\bin\\win64\\tier0.dll"),
    R(L"\\bin\\win64\\engine2.dll"),
    R(L"\\bin\\win64\\steam_api64.dll"),
    R(L"\\bin\\win64\\steamnetworkingsockets.dll"),
    R(L"\\bin\\win64\\dbghelp.dll"),
    R(L"\\bin\\win64\\vconcomm.dll"),
    R(L"\\bin\\win64\\SDL3.dll"),
    R(L"\\bin\\win64\\filesystem_stdio.dll"),
    R(L"\\bin\\win64\\inputsystem.dll"),
    R(L"\\bin\\win64\\imemanager.dll"),
    R(L"\\bin\\win64\\localize.dll"),
    R(L"\\bin\\win64\\rendersystemdx11.dll"),
    R(L"\\bin\\win64\\amd_ags_x64.dll"),
    R(L"\\bin\\win64\\d3dcompiler_47.dll"),
    R(L"\\bin\\win64\\resourcesystem.dll"),
    R(L"\\bin\\win64\\schemasystem.dll"),
    R(L"\\bin\\win64\\materialsystem2.dll"),
    R(L"\\bin\\win64\\meshsystem.dll"),
    R(L"\\bin\\win64\\worldrenderer.dll"),
    R(L"\\bin\\win64\\pulse_system.dll"),
    R(L"\\bin\\win64\\vscript.dll"),
    R(L"\\bin\\win64\\networksystem.dll"),
    R(L"\\bin\\win64\\animationsystem.dll"),
    R(L"\\bin\\win64\\vphysics2.dll"),
    R(L"\\bin\\win64\\soundsystem.dll"),
    R(L"\\bin\\win64\\steamaudio.dll"),
    R(L"\\bin\\win64\\phonon.dll"),
    R(L"\\bin\\win64\\scenesystem.dll"),
    R(L"\\bin\\win64\\particles.dll"),
    R(L"\\bin\\win64\\panoramauiclient.dll"),
    R(L"\\bin\\win64\\video64.dll"),
    R(L"\\bin\\win64\\libavformat-58.dll"),
    R(L"\\bin\\win64\\libavcodec-58.dll"),
    R(L"\\bin\\win64\\libavresample-4.dll"),
    R(L"\\bin\\win64\\libavutil-56.dll"),
    R(L"\\bin\\win64\\libswscale-5.dll"),
    R(L"\\bin\\win64\\v8.dll"),
    R(L"\\bin\\win64\\v8_libbase.dll"),
    R(L"\\bin\\win64\\v8_libplatform.dll"),
    R(L"\\bin\\win64\\v8_icui18n.dll"),
    R(L"\\bin\\win64\\v8_icuuc.dll"),
    R(L"\\bin\\win64\\v8_zlib.dll"),
    R(L"\\bin\\win64\\v8system.dll"),
    R(L"\\bin\\win64\\panorama.dll"),
    R(L"\\bin\\win64\\panorama_text_pango.dll"),
    R(L"\\bin\\win64\\libgobject-2.0-0.dll"),
    R(L"\\bin\\win64\\libglib-2.0-0.dll"),
    R(L"\\bin\\win64\\libgmodule-2.0-0.dll"),
    R(L"\\bin\\win64\\libfontconfig-1.dll"),
    R(L"\\bin\\win64\\libfreetype-6.dll"),
    R(L"\\bin\\win64\\libpango-1.0-0.dll"),
    R(L"\\bin\\win64\\libpangoft2-1.0-0.dll"),
    R(L"\\bin\\win64\\cairo.dll"),
    R(L"\\bin\\win64\\embree3.dll"),
    R(L"\\bin\\win64\\libmpg123-0.dll"),
    R(L"\\bin\\win64\\scenefilecache.dll"),
    R(L"\\bin\\win64\\navsystem.dll"),
    R(L"\\csgo\\bin\\win64\\client.dll"),
    R(L"\\csgo\\bin\\win64\\server.dll"),
    R(L"\\csgo\\bin\\win64\\host.dll"),
    R(L"\\csgo\\bin\\win64\\matchmaking.dll"),
};

static const UNICODE_STRING g_SteamRel[] = {
    R(L"\\GameOverlayRenderer64.dll"),
    R(L"\\steamclient64.dll"),
    R(L"\\tier0_s64.dll"),
    R(L"\\vstdlib_s64.dll"),
    R(L"\\crashhandler64.dll"),
};

static const UNICODE_STRING g_Sys32Rel[] = {
    R(L"\\ntdll.dll"),               R(L"\\kernel32.dll"),            R(L"\\KernelBase.dll"),
    R(L"\\apphelp.dll"),             R(L"\\AcLayers.dll"),            R(L"\\AcGenral.dll"),
    R(L"\\version.dll"),             R(L"\\userenv.dll"),             R(L"\\mpr.dll"),
    R(L"\\sspicli.dll"),             R(L"\\cfgmgr32.dll"),            R(L"\\winmm.dll"),
    R(L"\\msasn1.dll"),              R(L"\\bcrypt.dll"),              R(L"\\IPHLPAPI.DLL"),
    R(L"\\mswsock.dll"),             R(L"\\secur32.dll"),             R(L"\\cryptbase.dll"),
    R(L"\\wininet.dll"),             R(L"\\kernel.appcore.dll"),      R(L"\\uxtheme.dll"),
    R(L"\\hid.dll"),                 R(L"\\dwmapi.dll"),              R(L"\\dxgi.dll"),
    R(L"\\powrprof.dll"),            R(L"\\umpdc.dll"),               R(L"\\DXCore.dll"),
    R(L"\\ResourcePolicyClient.dll"),R(L"\\directxdatabasehelper.dll"),R(L"\\KBDUS.DLL"),
    R(L"\\d3d11.dll"),               R(L"\\d3d9.dll"),                R(L"\\cryptnet.dll"),
    R(L"\\wldp.dll"),                R(L"\\drvstore.dll"),            R(L"\\devobj.dll"),
    R(L"\\windows.storage.dll"),     R(L"\\atidxx64.dll"),            R(L"\\wtsapi32.dll"),
    R(L"\\propsys.dll"),             R(L"\\pdh.dll"),                 R(L"\\mscms.dll"),
    R(L"\\TextInputFramework.dll"),  R(L"\\DataExchange.dll"),        R(L"\\twinapi.appcore.dll"),
    R(L"\\CoreMessaging.dll"),       R(L"\\CoreUIComponents.dll"),    R(L"\\WinTypes.dll"),
    R(L"\\dcomp.dll"),               R(L"\\Microsoft.Internal.WarpPal.dll"),
    R(L"\\MMDevAPI.dll"),            R(L"\\AudioSes.dll"),            R(L"\\Windows.UI.dll"),
    R(L"\\wsock32.dll"),             R(L"\\usp10.dll"),               R(L"\\vcruntime140.dll"),
    R(L"\\vcruntime140_1.dll"),      R(L"\\msvcp140.dll"),            R(L"\\avrt.dll"),
    R(L"\\shfolder.dll"),            R(L"\\urlmon.dll"),              R(L"\\iertutil.dll"),
    R(L"\\netutils.dll"),            R(L"\\srvcli.dll"),              R(L"\\virtdisk.dll"),
    R(L"\\profapi.dll"),             R(L"\\Windows.System.Launcher.dll"),
    R(L"\\networkexplorer.dll"),
};

static VOID SetRoot(PUNICODE_STRING Root, PCUNICODE_STRING Path, PCUNICODE_STRING Rel)
{
    if (!RtlSuffixUnicodeString(Rel, Path, TRUE)) return;
    USHORT len = Path->Length - Rel->Length;
    PWCH buf = (PWCH)ExAllocatePool2(POOL_FLAG_NON_PAGED, len, 'tRaS');
    if (!buf) return;
    RtlCopyMemory(buf, Path->Buffer, len);
    Root->Buffer = buf;
    Root->Length = Root->MaximumLength = len;
}

static VOID FreeRoot(PUNICODE_STRING Root)
{
    if (Root->Buffer) ExFreePoolWithTag(Root->Buffer, 'tRaS');
    RtlZeroMemory(Root, sizeof(*Root));
}

static BOOLEAN MatchSet(PCUNICODE_STRING Path, PCUNICODE_STRING Root,
                        const UNICODE_STRING *Set, ULONG Count)
{
    USHORT rl = Root->Length;
    if (!rl || Path->Length <= rl) return FALSE;

    UNICODE_STRING head = { rl, rl, Path->Buffer };
    if (!RtlEqualUnicodeString(&head, Root, TRUE)) return FALSE;

    USHORT tl = (USHORT)(Path->Length - rl);
    UNICODE_STRING tail = { tl, tl, Path->Buffer + rl / sizeof(WCHAR) };
    for (ULONG i = 0; i < Count; i++)
        if (RtlEqualUnicodeString(&tail, &Set[i], TRUE)) return TRUE;
    return FALSE;
}

static BOOLEAN IsAllowlisted(PCUNICODE_STRING Path)
{
    return MatchSet(Path, &g_GameRoot,  g_GameRel,  ARRAYSIZE(g_GameRel))
        || MatchSet(Path, &g_Sys32Root, g_Sys32Rel, ARRAYSIZE(g_Sys32Rel))
        || MatchSet(Path, &g_SteamRoot, g_SteamRel, ARRAYSIZE(g_SteamRel));
}