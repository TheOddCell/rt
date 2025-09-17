pkgname=r00t
pkgver=2.0.0
pkgrel=1
pkgdesc="rt - root, das it"
arch=('x86_64')
url="https://github.com/theoddcell/rt"
license=('MIT')
depends=()
makedepends=('gcc')
source=('rt.c')
sha256sums=('SKIP')

build() {
  cd "$srcdir"
  gcc -O2 -Wall -std=gnu11 -o rt rt.c -lpam -lpam_misc
}

package() {
    install -Dm755 rt "$pkgdir/usr/bin/rt"
    chmod 4750 "$pkgdir/usr/bin/rt"
    chown root:wheel "$pkgdir/usr/bin/rt"
}
