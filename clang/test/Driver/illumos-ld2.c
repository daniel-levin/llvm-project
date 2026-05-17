// RUN: %clang -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s
// RUN: %clang -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s
// CHECK-NOT: warning:
// CHECK:     "-C"
// CHECK:     crt1.o
// CHECK:     crti.o
// CHECK:     values-Xa.o
// CHECK:     values-xpg6.o
// CHECK:     crtbegin.o
// CHECK:     -lgcc
// CHECK:     -lc
// CHECK:     crtend.o
// CHECK:     crtn.o
// CHECK-NOT: values-Xc.o
// CHECK-NOT: values-xpg4.o
