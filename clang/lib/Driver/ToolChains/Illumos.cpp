//===--- Illumos.cpp - Illumos ToolChain Implementations --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Illumos.h"
#include "Gnu.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Config/config.h"
#include "clang/Driver/CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/SanitizerArgs.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Options/Options.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

void illumos::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                   const InputInfo &Output,
                                   const InputInfoList &Inputs,
                                   const ArgList &Args,
                                   const char *LinkingOutput) const {
  const auto &ToolChain = static_cast<const Illumos &>(getToolChain());
  const LinkBlueprints &LB = ToolChain.getLinkBlueprints();
  const llvm::Triple::ArchType Arch = ToolChain.getArch();

  ArgStringList CmdArgs;

  // Some command line arguments are the same on the system linker and GNU
  // ld-compatible linkers.
  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  }

  if (LB.UsingSystemLinker && LB.DemangleSymbols) {
    // Demangles C++ symbols, on by default in GNU ld compatible linkers.
    CmdArgs.push_back("-C");
  } else if (!LB.UsingSystemLinker && !LB.DemangleSymbols) {
    CmdArgs.push_back("--no-demangle");
  }

  if (LB.shouldSetStartSymbol()) {
    CmdArgs.push_back("-e");
    CmdArgs.push_back("_start");
  }

  if (LB.GenerateRelocatableObject) {
    CmdArgs.push_back("-r");
  }

  // The system linker determines which architecture to use based
  // on the ELF class and machine type of the first relocatable
  // object passed to it. It has no option for setting the architecture
  // any other way. By contrast, GNU-ld compatible linkers must be told
  // which architecture they're creating an artifact for. The so-called
  // emulation flag is used to determine, amongst other things, the choice
  // of dynamic linker and final layout.
  if (!LB.UsingSystemLinker) {
    switch (Arch) {
    case llvm::Triple::x86:
      CmdArgs.push_back("-m");
      CmdArgs.push_back("elf_i386_sol2");
      break;
    case llvm::Triple::x86_64:
      CmdArgs.push_back("-m");
      CmdArgs.push_back("elf_x86_64_sol2");
      break;
    default:
      break;
    }

    CmdArgs.push_back("--eh-frame-hdr");
  }

  if (LB.ExportDynamic) {
    if (LB.UsingSystemLinker) {
      // The system linker has no flag for this.
      // Leaving this empty branch here for clarity.
    } else {
      CmdArgs.push_back("--export-dynamic");
    }
  }

  if (LB.shouldGeneratePIE()) {
    if (LB.UsingSystemLinker) {
      CmdArgs.push_back("-z");
      CmdArgs.push_back("type=pie");
    } else {
      CmdArgs.push_back("-pie");
    }
  }

  if (LB.GenerateStatic) {
    if (LB.UsingSystemLinker) {
      CmdArgs.push_back("-Bstatic");
      CmdArgs.push_back("-dn");
    } else {
      CmdArgs.push_back("-static");
    }
  }

  if (LB.LinkLibc && LB.LinkStartFiles) {
    if (!LB.GenerateShared) {
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crt1.o")));
    }
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crti.o")));

    if (LB.HaveAnsi || (LB.Std && !LB.Std->isGNUMode())) {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("values-Xc.o")));
    } else {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("values-Xa.o")));
    }

    if (LB.Std && LB.Std->getLanguage() == Language::C && !LB.Std->isC99()) {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("values-xpg4.o")));
    } else {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("values-xpg6.o")));
    }

    if (LB.shouldGeneratePIE()) {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("crtbeginS.o")));
    } else {
      CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("crtbegin.o")));
    }
  }

  ToolChain.AddFilePathLibArgs(Args, CmdArgs);
  Args.addAllArgs(CmdArgs, {options::OPT_L, options::OPT_T_Group});
  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs, JA);

  /// TODO: sanitizers, libm, openmp, -fstack_protector

  AddRunTimeLibs(ToolChain, ToolChain.getDriver(), CmdArgs, Args);

  if (LB.LinkLibc) {
    CmdArgs.push_back("-lc");

    if (LB.LinkStartFiles) {
      if (LB.shouldGeneratePIE()) {
        CmdArgs.push_back(
            Args.MakeArgString(ToolChain.GetFilePath("crtendS.o")));
      } else {
        CmdArgs.push_back(
            Args.MakeArgString(ToolChain.GetFilePath("crtend.o")));
      }

      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crtn.o")));
    }
  }

  const char *Exec = Args.MakeArgString(LB.LinkerPath);
  C.addCommand(std::make_unique<Command>(JA, *this, ResponseFileSupport::None(),
                                         Exec, CmdArgs, Inputs, Output));
}

Illumos::Illumos(const Driver &D, const llvm::Triple &Triple,
                 const ArgList &Args)
    : Generic_ELF(D, Triple, Args), LinkBlueprints() {

  GCCInstallation.init(Triple, Args);

  if (GCCInstallation.isValid()) {
    addPathIfExists(D,
                    concat(GCCInstallation.getInstallPath(),
                           GCCInstallation.getMultilib().gccSuffix()),
                    getFilePaths());
  }

  switch (Triple.getArch()) {
  case llvm::Triple::x86:
    addPathIfExists(D, concat(concat(D.SysRoot, "/usr"), "/lib"),
                    getFilePaths());
    break;
  case llvm::Triple::x86_64:
    addPathIfExists(D,
                    concat(concat(concat(D.SysRoot, "/usr"), "/lib"), "/amd64"),
                    getFilePaths());
    break;
  default:
    break;
  }

  LinkBlueprints.DemangleSymbols =
      !Args.hasArg(options::OPT_Z_Xlinker__no_demangle);

  LinkBlueprints.GenerateRelocatableObject = Args.hasArg(options::OPT_r);

  LinkBlueprints.GenerateShared = Args.hasArg(options::OPT_shared);

  LinkBlueprints.PIERequested = Args.hasArg(options::OPT_pie);

  LinkBlueprints.GenerateStatic = Args.hasArg(options::OPT_static);

  if (Args.hasArg(options::OPT_rdynamic)) {
    LinkBlueprints.ExportDynamic = true;
    Args.ClaimAllArgs(options::OPT_rdynamic);
  }

  if (Args.hasArg(options::OPT_nostdlib)) {
    LinkBlueprints.LinkLibc = false;
  }

  if (Args.hasArg(options::OPT_nostartfiles)) {
    LinkBlueprints.LinkStartFiles = false;
  }

  const Arg *Std = Args.getLastArg(options::OPT_std_EQ, options::OPT_ansi);
  if (Std) {
    LinkBlueprints.HaveAnsi = Std->getOption().matches(options::OPT_ansi);
    if (!LinkBlueprints.HaveAnsi)
      LinkBlueprints.Std =
          LangStandard::getLangStandardForName(Std->getValue());
  }

  if (const Arg *A = Args.getLastArg(options::OPT_fuse_ld_EQ)) {
    StringRef UseLinker = A->getValue();
    if (!UseLinker.empty()) {
      Args.ClaimAllArgs(options::OPT_fuse_ld_EQ);
      LinkBlueprints.LinkerPath = UseLinker;
    }
  }
}

bool Illumos::mustElideDynamicList() const {
  return LinkBlueprints.UsingSystemLinker;
}

SanitizerMask Illumos::getSupportedSanitizers() const {
  /// TODO: ensure sanitizers work on Illumos
  return ToolChain::getSupportedSanitizers();
}

void Illumos::addAsNeededOption(llvm::opt::ArgStringList &CmdArgs,
                                bool as_needed) const {
  if (LinkBlueprints.UsingSystemLinker) {
    CmdArgs.push_back("-z");
    CmdArgs.push_back(as_needed ? "ignore" : "record");
  } else {
    CmdArgs.push_back(as_needed ? "--as-needed" : "--no-as-needed");
  }
}

void Illumos::addLibStdCxxIncludePaths(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {

  if (!GCCInstallation.isValid())
    return;

  StringRef LibDir = GCCInstallation.getParentLibPath();
  StringRef TripleStr = GCCInstallation.getTriple().str();
  const Multilib &Multilib = GCCInstallation.getMultilib();
  const GCCVersion &Version = GCCInstallation.getVersion();

  addLibStdCXXIncludePaths(LibDir.str() + "/../include/c++/" + Version.Text,
                           TripleStr, Multilib.includeSuffix(), DriverArgs,
                           CC1Args);
}

void Illumos::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                        ArgStringList &CC1Args) const {
  const Driver &D = getDriver();

  if (DriverArgs.hasArg(options::OPT_nostdinc))
    return;

  if (!DriverArgs.hasArg(options::OPT_nostdlibinc))
    addSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/usr/local/include");

  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    SmallString<128> P(D.ResourceDir);
    llvm::sys::path::append(P, "include");
    addSystemInclude(DriverArgs, CC1Args, P);
  }

  if (DriverArgs.hasArg(options::OPT_nostdlibinc))
    return;

  // Check for configure-time C include directories.
  StringRef CIncludeDirs(C_INCLUDE_DIRS);
  if (CIncludeDirs != "") {
    SmallVector<StringRef, 5> dirs;
    CIncludeDirs.split(dirs, ":");
    for (StringRef dir : dirs) {
      StringRef Prefix =
          llvm::sys::path::is_absolute(dir) ? "" : StringRef(D.SysRoot);
      addExternCSystemInclude(DriverArgs, CC1Args, Prefix + dir);
    }
    return;
  }

  // Add include directories specific to the selected multilib set and multilib.
  if (GCCInstallation.isValid()) {
    const MultilibSet::IncludeDirsFunc &Callback =
        Multilibs.includeDirsCallback();
    if (Callback) {
      for (const auto &Path : Callback(GCCInstallation.getMultilib()))
        addExternCSystemIncludeIfExists(
            DriverArgs, CC1Args, GCCInstallation.getInstallPath() + Path);
    }
  }

  addExternCSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/usr/include");
}
