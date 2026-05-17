//===--- Illumos.h - Illumos ToolChain Implementations ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ILLUMOS_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ILLUMOS_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace illumos {

class LLVM_LIBRARY_VISIBILITY Assembler final : public gnutools::Assembler {
public:
  Assembler(const ToolChain &TC) : gnutools::Assembler(TC) {
    DefaultAssembler = "gas";
  }

  bool hasIntegratedCPP() const override { return false; }
};

/// Illumos has a Solaris-descended linker known as the "Sun Link Editor".
/// We also support any GNU ld compatible linker, such as Wild and Mold.
class LLVM_LIBRARY_VISIBILITY Linker final : public Tool {

public:
  Linker(const ToolChain &TC) : Tool("illumos::Linker", "linker", TC) {}

  bool hasIntegratedCPP() const override { return false; }

  bool isLinkJob() const override { return true; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;
};

/// Holds enough data to drive the linker compatibly with either the
/// system linker or any GNU ld-compatible linker. We separate parsing our
/// own arguments from generating the actual command to the linker.
struct LLVM_LIBRARY_VISIBILITY LinkBlueprints {
  bool UsingSystemLinker;
  std::string LinkerPath;
  bool GeneratePIE;
  bool ExportDynamic;

public:
  LinkBlueprints() {
    UsingSystemLinker = true;
    LinkerPath = "/usr/bin/ld";
    GeneratePIE = false;
    ExportDynamic = false;
  }
};

} // end namespace illumos
} // end namespace tools

namespace toolchains {

/// For the most part, linking binaries on Illumos works like other Unix
/// and Unix-like operating systems:
///
/// 1. Identify C runtime object files, provided by the OS. Typically this
///    is known as the 'sysroot', although this term is overloaded
///    and not standardized.
/// 2. Identify libgcc and the objects it provides.
/// 3. Choose a linker.
/// 4. Construct an appropriate command line invocation for the
///    chosen linker, given our current compilation job and options.
///
/// Illumos is a descendent of OpenSolaris, which itself descends from
/// SVR4, which was a merger of SunOS and AT&T Unix. Each Illumos ancestor
/// has contributed to every one of the steps outlined above. That is why
/// it needs a specific ToolChain: it differs from all others uniquely
/// and often subtly.
///
/// Over time, Illumos has evolved to the point where it is no longer compatible
/// with Oracle Solaris, its closed-source nearest relative. Despite this, we
/// can hang off the existing generic ELF facilities in the compiler, and
/// only address Illumos-specifics as and when they arise.
///
/// We confine Illumos-specific driver functionality here, so that it does
/// not externalize its needs onto the rest of the driver.
class LLVM_LIBRARY_VISIBILITY Illumos : public Generic_ELF {
  tools::illumos::LinkBlueprints LinkBlueprints;

public:
  Illumos(const Driver &D, const llvm::Triple &Triple,
          const llvm::opt::ArgList &Args);

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;

  void
  addLibStdCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
                           llvm::opt::ArgStringList &CC1Args) const override;

  /// The Illumos linker supports an *equivalent* of --as-needed/--no-as-needed,
  /// but needs specific instructions to do do. We handle that here.
  void addAsNeededOption(llvm::opt::ArgStringList &CmdArgs,
                         bool as_needed) const override;

  SanitizerMask getSupportedSanitizers() const override;

  /// The system linker does not support this option, so we elide it unless
  /// using a GNU ld-compatible linker.
  virtual bool mustElideDynamicList() const override;

  /// Unlike Linux, Illumos ships its kernel and userland as a single, coupled
  /// unit. As such we can happily take dependencies on particular programs
  /// existing at specific paths. In this case, we take a dependency on the
  /// system linker, whose command line interface is different from all others.
  const char *getDefaultLinker() const override { return "/usr/bin/ld"; }

  const tools::illumos::LinkBlueprints &getLinkBlueprints() const {
    return LinkBlueprints;
  }

protected:
  Tool *buildAssembler() const override {
    return new tools::illumos::Assembler(*this);
  }

  Tool *buildLinker() const override {
    return new tools::illumos::Linker(*this);
  }
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ILLUMOS_H
