package=curl
$(package)_version=7.79.1
$(package)_download_path=https://curl.haxx.se/download/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=370b11201349816287fb0ccc995e420277fbfcaf76206e309b3f60f0eda090c2
$(package)_dependencies=openssl zlib

define $(package)_set_vars
$(package)_config_opts=--disable-shared --with-ca-fallback
$(package)_config_opts+=--disable-cookies
$(package)_config_opts+=--disable-manual
$(package)_config_opts+=--disable-unix-sockets
$(package)_config_opts+=--disable-verbose
$(package)_config_opts+=--disable-versioned-symbols
$(package)_config_opts+=--enable-symbol-hiding
$(package)_config_opts+=--without-librtmp
$(package)_config_opts+=--disable-rtsp
$(package)_config_opts+=--disable-alt-svc
$(package)_config_opts_mingw32=--enable-sspi --without-ssl --with-nss --with-schannel
$(package)_config_opts_darwin=--enable-sspi --without-ssl --with-secure-transport
$(package)_config_opts_linux=--with-ssl
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
