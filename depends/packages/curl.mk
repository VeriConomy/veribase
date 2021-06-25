package=curl
$(package)_version=7.77.0
$(package)_download_path=https://curl.haxx.se/download/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=b0a3428acb60fa59044c4d0baae4e4fc09ae9af1d8a3aa84b2e3fbcd99841f77
$(package)_dependencies=openssl zlib

define $(package)_set_vars
$(package)_config_opts=--disable-shared --with-ca-fallback
$(package)_config_opts_mingw32=--enable-sspi --without-ssl --with-nss --with-schannel
$(package)_config_opts_darwin=--enable-sspi --without-ssl --with-secure-transport
$(package)_config_opts_linux=--with-openssl
$(package)_cflags=-DCURL_STATICLIB
$(package)_cxxflags=-std=c++11
endef

define $(package)_config_cmds
  autoreconf -fi && \
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

