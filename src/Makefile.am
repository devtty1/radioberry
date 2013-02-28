bin_PROGRAMS = radioberry

radioberry_SOURCES = mcp32xx.c spi_base.c set_volume.c set_volume_alsa.c set_tuner.c mpd_ctl.c radioberry.c set_volume_mpd.c gpio_base.c lcd_ctl.c
radioberry_CPPFLAGS =
radioberry_CPPFLAGS += $(SYSROOT_FLAG)
radioberry_LDADD =
radioberry_LDADD += $(MPD_LIB)
radioberry_LDADD += $(CONFUSE_LIB)
radioberry_LDADD += $(ASOUND_LIB)


