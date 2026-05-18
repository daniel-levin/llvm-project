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
// CHECK:     "-e"
// CHECK:     "_start"
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

// RUN: %clang -ansi -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-ANSI
// RUN: %clang -ansi -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-ANSI
// CHECK-ANSI-NOT: warning:
// CHECK-ANSI:     "-C"
// CHECK-ANSI:     "-e"
// CHECK-ANSI:     "_start"
// CHECK-ANSI:     crt1.o
// CHECK-ANSI:     crti.o
// CHECK-ANSI:     values-Xc.o
// CHECK-ANSI:     values-xpg6.o
// CHECK-ANSI:     crtbegin.o
// CHECK-ANSI:     -lgcc
// CHECK-ANSI:     -lc
// CHECK-ANSI:     crtend.o
// CHECK-ANSI:     crtn.o
// CHECK-ANSI-NOT: values-Xa.o
// CHECK-ANSI-NOT: values-xpg4.o

// RUN: %clang -std=c89 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-C89
// RUN: %clang -std=c89 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-C89
// CHECK-C89-NOT: warning:
// CHECK-C89:     "-C"
// CHECK-C89:     "-e"
// CHECK-C89:     "_start"
// CHECK-C89:     crt1.o
// CHECK-C89:     crti.o
// CHECK-C89:     values-Xc.o
// CHECK-C89:     values-xpg4.o
// CHECK-C89:     crtbegin.o
// CHECK-C89:     -lgcc
// CHECK-C89:     -lc
// CHECK-C89:     crtend.o
// CHECK-C89:     crtn.o
// CHECK-C89-NOT: values-Xa.o
// CHECK-C89-NOT: values-xpg6.o

// RUN: %clang -std=c90 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-C90
// RUN: %clang -std=c90 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-C90
// CHECK-C90-NOT: warning:
// CHECK-C90:     "-C"
// CHECK-C90:     "-e"
// CHECK-C90:     "_start"
// CHECK-C90:     crt1.o
// CHECK-C90:     crti.o
// CHECK-C90:     values-Xc.o
// CHECK-C90:     values-xpg4.o
// CHECK-C90:     crtbegin.o
// CHECK-C90:     -lgcc
// CHECK-C90:     -lc
// CHECK-C90:     crtend.o
// CHECK-C90:     crtn.o
// CHECK-C90-NOT: values-Xa.o
// CHECK-C90-NOT: values-xpg6.o

// RUN: %clang -std=iso9899:199409 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-C94
// RUN: %clang -std=iso9899:199409 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-C94
// CHECK-C94-NOT: warning:
// CHECK-C94:     "-C"
// CHECK-C94:     "-e"
// CHECK-C94:     "_start"
// CHECK-C94:     crt1.o
// CHECK-C94:     crti.o
// CHECK-C94:     values-Xc.o
// CHECK-C94:     values-xpg4.o
// CHECK-C94:     crtbegin.o
// CHECK-C94:     -lgcc
// CHECK-C94:     -lc
// CHECK-C94:     crtend.o
// CHECK-C94:     crtn.o
// CHECK-C94-NOT: values-Xa.o
// CHECK-C94-NOT: values-xpg6.o

// RUN: %clang -std=c11 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-C11
// RUN: %clang -std=c11 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-C11
// CHECK-C11-NOT: warning:
// CHECK-C11:     "-C"
// CHECK-C11:     "-e"
// CHECK-C11:     "_start"
// CHECK-C11:     crt1.o
// CHECK-C11:     crti.o
// CHECK-C11:     values-Xc.o
// CHECK-C11:     values-xpg6.o
// CHECK-C11:     crtbegin.o
// CHECK-C11:     -lgcc
// CHECK-C11:     -lc
// CHECK-C11:     crtend.o
// CHECK-C11:     crtn.o
// CHECK-C11-NOT: values-Xa.o
// CHECK-C11-NOT: values-xpg4.o

// RUN: %clang -std=gnu89 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU89
// RUN: %clang -std=gnu89 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU89
// CHECK-GNU89-NOT: warning:
// CHECK-GNU89:     "-C"
// CHECK-GNU89:     "-e"
// CHECK-GNU89:     "_start"
// CHECK-GNU89:     crt1.o
// CHECK-GNU89:     crti.o
// CHECK-GNU89:     values-Xa.o
// CHECK-GNU89:     values-xpg4.o
// CHECK-GNU89:     crtbegin.o
// CHECK-GNU89:     -lgcc
// CHECK-GNU89:     -lc
// CHECK-GNU89:     crtend.o
// CHECK-GNU89:     crtn.o
// CHECK-GNU89-NOT: values-Xc.o
// CHECK-GNU89-NOT: values-xpg6.o

// RUN: %clang -std=gnu90 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU90
// RUN: %clang -std=gnu90 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU90
// CHECK-GNU90-NOT: warning:
// CHECK-GNU90:     "-C"
// CHECK-GNU90:     "-e"
// CHECK-GNU90:     "_start"
// CHECK-GNU90:     crt1.o
// CHECK-GNU90:     crti.o
// CHECK-GNU90:     values-Xa.o
// CHECK-GNU90:     values-xpg4.o
// CHECK-GNU90:     crtbegin.o
// CHECK-GNU90:     -lgcc
// CHECK-GNU90:     -lc
// CHECK-GNU90:     crtend.o
// CHECK-GNU90:     crtn.o
// CHECK-GNU90-NOT: values-Xc.o
// CHECK-GNU90-NOT: values-xpg6.o

// RUN: %clang -std=gnu11 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_omnios \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU11
// RUN: %clang -std=gnu11 -### %s 2>&1 \
// RUN:     --target=x86_64-pc-illumos \
// RUN:     --sysroot=%S/Inputs/illumos_x86_tree_openindiana \
// RUN:   | FileCheck %s --check-prefix=CHECK-GNU11
// CHECK-GNU11-NOT: warning:
// CHECK-GNU11:     "-C"
// CHECK-GNU11:     "-e"
// CHECK-GNU11:     "_start"
// CHECK-GNU11:     crt1.o
// CHECK-GNU11:     crti.o
// CHECK-GNU11:     values-Xa.o
// CHECK-GNU11:     values-xpg6.o
// CHECK-GNU11:     crtbegin.o
// CHECK-GNU11:     -lgcc
// CHECK-GNU11:     -lc
// CHECK-GNU11:     crtend.o
// CHECK-GNU11:     crtn.o
// CHECK-GNU11-NOT: values-Xc.o
// CHECK-GNU11-NOT: values-xpg4.o
