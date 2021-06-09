package=minizip
$(package)_version=1.1
$(package)_download_path=https://codeload.github.com/nmoinvaz/minizip/tar.gz
$(package)_file_name=1.1
$(package)_sha256_hash=2b09372e0a762fe81cebbcced6360385dccd09267ba1cbdb4b35cf6009bb6381
$(package)_dependencies=zlib

define $(package)_set_vars
$(package)_config_opts=--enable-cxx --disable-shared --enable-static
$(package)_config_opts_linux=--with-pic

ifeq ($(host),x86_64-apple-darwin16)
$(package)_cc=$(BASEDIR)/$(host)/native/bin/clang
$(package)_cxx=$(BASEDIR)/$(host)/native/bin/clang++
$(package)_cxxflags=-fPIC
$(package)_cflags=-fPIC
$(package)_cppflags=-fPIC
endif
endef

define $(package)_config_cmds
  autoreconf -i && \
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
