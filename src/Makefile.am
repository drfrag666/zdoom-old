
bin_PROGRAMS = zdoom

AM_CPPFLAGS = \
	-Isrc \
	-I$(srcdir)/sound \
	-I$(srcdir)/sdl \
	-I$(srcdir)/g_shared \
	-I$(srcdir)/g_doom \
	-I$(srcdir)/g_raven \
	-I$(srcdir)/g_heretic \
	-I$(srcdir)/g_hexen \
	-I$(srcdir)/g_strife \
	-I$(srcdir)/oplsynth \
	-I$(srcdir)/sdl \
	-DSHARE_DIR=\"$(pkgdatadir)/\"
	
zdoom_SOURCES = \
	$(srcdir)/autostart.cpp \
	$(srcdir)/am_map.cpp \
	$(srcdir)/b_bot.cpp \
	$(srcdir)/b_func.cpp \
	$(srcdir)/b_game.cpp \
	$(srcdir)/b_move.cpp \
	$(srcdir)/b_think.cpp \
	$(srcdir)/bbannouncer.cpp \
	$(srcdir)/c_bind.cpp \
	$(srcdir)/c_cmds.cpp \
	$(srcdir)/c_console.cpp \
	$(srcdir)/c_cvars.cpp \
	$(srcdir)/c_dispatch.cpp \
	$(srcdir)/c_expr.cpp \
	$(srcdir)/cmdlib.cpp \
	$(srcdir)/colormatcher.cpp \
	$(srcdir)/configfile.cpp \
	$(srcdir)/ct_chat.cpp \
	$(srcdir)/d_dehacked.cpp \
	$(srcdir)/d_main.cpp \
	$(srcdir)/d_net.cpp \
	$(srcdir)/d_netinfo.cpp \
	$(srcdir)/d_protocol.cpp \
	$(srcdir)/decallib.cpp \
	$(srcdir)/decorations.cpp \
	$(srcdir)/dobject.cpp \
	$(srcdir)/doomdef.cpp \
	$(srcdir)/doomstat.cpp \
	$(srcdir)/dsectoreffect.cpp \
	$(srcdir)/dthinker.cpp \
	$(srcdir)/f_finale.cpp \
	$(srcdir)/f_wipe.cpp \
	$(srcdir)/farchive.cpp \
	$(srcdir)/files.cpp \
	$(srcdir)/g_game.cpp \
	$(srcdir)/g_level.cpp \
	$(srcdir)/gameconfigfile.cpp \
	$(srcdir)/gi.cpp \
	$(srcdir)/hu_scores.cpp \
	$(srcdir)/info.cpp \
	$(srcdir)/infodefaults.cpp \
	$(srcdir)/lumpconfigfile.cpp \
	$(srcdir)/m_alloc.cpp \
	$(srcdir)/m_argv.cpp \
	$(srcdir)/m_bbox.cpp \
	$(srcdir)/m_cheat.cpp \
	$(srcdir)/m_fixed.cpp \
	$(srcdir)/m_menu.cpp \
	$(srcdir)/m_misc.cpp \
	$(srcdir)/m_options.cpp \
	$(srcdir)/m_png.cpp \
	$(srcdir)/m_random.cpp \
	$(srcdir)/mus2midi.cpp \
	$(srcdir)/nodebuild.cpp \
	$(srcdir)/nodebuild_events.cpp \
	$(srcdir)/nodebuild_extract.cpp \
	$(srcdir)/nodebuild_gl.cpp \
	$(srcdir)/nodebuild_utility.cpp \
	$(srcdir)/p_acs.cpp \
	$(srcdir)/p_buildmap.cpp \
	$(srcdir)/p_ceiling.cpp \
	$(srcdir)/p_conversation.cpp \
	$(srcdir)/p_doors.cpp \
	$(srcdir)/p_effect.cpp \
	$(srcdir)/p_enemy.cpp \
	$(srcdir)/p_floor.cpp \
	$(srcdir)/p_interaction.cpp \
	$(srcdir)/p_lights.cpp \
	$(srcdir)/p_lnspec.cpp \
	$(srcdir)/p_map.cpp \
	$(srcdir)/p_maputl.cpp \
	$(srcdir)/p_mobj.cpp \
	$(srcdir)/p_pillar.cpp \
	$(srcdir)/p_plats.cpp \
	$(srcdir)/p_pspr.cpp \
	$(srcdir)/p_saveg.cpp \
	$(srcdir)/p_sectors.cpp \
	$(srcdir)/p_setup.cpp \
	$(srcdir)/p_sight.cpp \
	$(srcdir)/p_spec.cpp \
	$(srcdir)/p_switch.cpp \
	$(srcdir)/p_teleport.cpp \
	$(srcdir)/p_terrain.cpp \
	$(srcdir)/p_things.cpp \
	$(srcdir)/p_tick.cpp \
	$(srcdir)/p_trace.cpp \
	$(srcdir)/p_user.cpp \
	$(srcdir)/p_writemap.cpp \
	$(srcdir)/p_xlat.cpp \
	$(srcdir)/po_man.cpp \
	$(srcdir)/s_advsound.cpp \
	$(srcdir)/s_environment.cpp \
	$(srcdir)/s_playlist.cpp \
	$(srcdir)/s_sndseq.cpp \
	$(srcdir)/s_sound.cpp \
	$(srcdir)/sc_man.cpp \
	$(srcdir)/skins.cpp \
	$(srcdir)/st_stuff.cpp \
	$(srcdir)/stats.cpp \
	$(srcdir)/stringtable.cpp \
	$(srcdir)/tables.cpp \
	$(srcdir)/tempfiles.cpp \
	$(srcdir)/thingdef.cpp \
	$(srcdir)/v_collection.cpp \
	$(srcdir)/v_draw.cpp \
	$(srcdir)/v_font.cpp \
	$(srcdir)/v_palette.cpp \
	$(srcdir)/v_pfx.cpp \
	$(srcdir)/v_text.cpp \
	$(srcdir)/v_video.cpp \
	$(srcdir)/vectors.cpp \
	$(srcdir)/w_wad.cpp \
	$(srcdir)/wi_stuff.cpp \
	$(srcdir)/a.nas \
	$(srcdir)/misc.nas \
	$(srcdir)/tmap.nas \
	$(srcdir)/tmap2.nas \
	$(srcdir)/tmap3.nas \
	$(srcdir)/g_shared/a_action.cpp \
	$(srcdir)/g_shared/a_artifacts.cpp \
	$(srcdir)/g_shared/a_bridge.cpp \
	$(srcdir)/g_shared/a_camera.cpp \
	$(srcdir)/g_shared/a_debris.cpp \
	$(srcdir)/g_shared/a_decals.cpp \
	$(srcdir)/g_shared/a_flashfader.cpp \
	$(srcdir)/g_shared/a_fountain.cpp \
	$(srcdir)/g_shared/a_hatetarget.cpp \
	$(srcdir)/g_shared/a_keys.cpp \
	$(srcdir)/g_shared/a_lightning.cpp \
	$(srcdir)/g_shared/a_movingcamera.cpp \
	$(srcdir)/g_shared/a_pickups.cpp \
	$(srcdir)/g_shared/a_quake.cpp \
	$(srcdir)/g_shared/a_secrettrigger.cpp \
	$(srcdir)/g_shared/a_sectoraction.cpp \
	$(srcdir)/g_shared/a_sharedmisc.cpp \
	$(srcdir)/g_shared/a_skies.cpp \
	$(srcdir)/g_shared/a_soundenvironment.cpp \
	$(srcdir)/g_shared/a_spark.cpp \
	$(srcdir)/g_shared/a_splashes.cpp \
	$(srcdir)/g_shared/a_waterzone.cpp \
	$(srcdir)/g_shared/a_weapons.cpp \
	$(srcdir)/g_shared/hudmessages.cpp \
	$(srcdir)/g_shared/shared_sbar.cpp \
	$(srcdir)/g_doom/a_arachnotron.cpp \
	$(srcdir)/g_doom/a_archvile.cpp \
	$(srcdir)/g_doom/a_bossbrain.cpp \
	$(srcdir)/g_doom/a_bruiser.cpp \
	$(srcdir)/g_doom/a_cacodemon.cpp \
	$(srcdir)/g_doom/a_cyberdemon.cpp \
	$(srcdir)/g_doom/a_demon.cpp \
	$(srcdir)/g_doom/a_doomarmor.cpp \
	$(srcdir)/g_doom/a_doomartifacts.cpp \
	$(srcdir)/g_doom/a_doomdecorations.cpp \
	$(srcdir)/g_doom/a_doomhealth.cpp \
	$(srcdir)/g_doom/a_doomimp.cpp \
	$(srcdir)/g_doom/a_doomkeys.cpp \
	$(srcdir)/g_doom/a_doommisc.cpp \
	$(srcdir)/g_doom/a_doomplayer.cpp \
	$(srcdir)/g_doom/a_doomweaps.cpp \
	$(srcdir)/g_doom/a_fatso.cpp \
	$(srcdir)/g_doom/a_keen.cpp \
	$(srcdir)/g_doom/a_lostsoul.cpp \
	$(srcdir)/g_doom/a_painelemental.cpp \
	$(srcdir)/g_doom/a_possessed.cpp \
	$(srcdir)/g_doom/a_revenant.cpp \
	$(srcdir)/g_doom/a_scriptedmarine.cpp \
	$(srcdir)/g_doom/a_spidermaster.cpp \
	$(srcdir)/g_doom/doom_sbar.cpp \
	$(srcdir)/g_raven/a_artiegg.cpp \
	$(srcdir)/g_raven/a_artitele.cpp \
	$(srcdir)/g_raven/a_minotaur.cpp \
	$(srcdir)/g_raven/a_ravenambient.cpp \
	$(srcdir)/g_raven/a_ravenartifacts.cpp \
	$(srcdir)/g_raven/a_ravenhealth.cpp \
	$(srcdir)/g_heretic/a_beast.cpp \
	$(srcdir)/g_heretic/a_chicken.cpp \
	$(srcdir)/g_heretic/a_clink.cpp \
	$(srcdir)/g_heretic/a_dsparil.cpp \
	$(srcdir)/g_heretic/a_hereticambience.cpp \
	$(srcdir)/g_heretic/a_hereticarmor.cpp \
	$(srcdir)/g_heretic/a_hereticartifacts.cpp \
	$(srcdir)/g_heretic/a_hereticdecorations.cpp \
	$(srcdir)/g_heretic/a_hereticimp.cpp \
	$(srcdir)/g_heretic/a_heretickeys.cpp \
	$(srcdir)/g_heretic/a_hereticmisc.cpp \
	$(srcdir)/g_heretic/a_hereticplayer.cpp \
	$(srcdir)/g_heretic/a_hereticweaps.cpp \
	$(srcdir)/g_heretic/a_ironlich.cpp \
	$(srcdir)/g_heretic/a_knight.cpp \
	$(srcdir)/g_heretic/a_mummy.cpp \
	$(srcdir)/g_heretic/a_snake.cpp \
	$(srcdir)/g_heretic/a_wizard.cpp \
	$(srcdir)/g_heretic/heretic_sbar.cpp \
	$(srcdir)/g_hexen/a_bats.cpp \
	$(srcdir)/g_hexen/a_bishop.cpp \
	$(srcdir)/g_hexen/a_blastradius.cpp \
	$(srcdir)/g_hexen/a_boostarmor.cpp \
	$(srcdir)/g_hexen/a_centaur.cpp \
	$(srcdir)/g_hexen/a_clericboss.cpp \
	$(srcdir)/g_hexen/a_clericflame.cpp \
	$(srcdir)/g_hexen/a_clericholy.cpp \
	$(srcdir)/g_hexen/a_clericmace.cpp \
	$(srcdir)/g_hexen/a_clericplayer.cpp \
	$(srcdir)/g_hexen/a_clericstaff.cpp \
	$(srcdir)/g_hexen/a_demons.cpp \
	$(srcdir)/g_hexen/a_dragon.cpp \
	$(srcdir)/g_hexen/a_ettin.cpp \
	$(srcdir)/g_hexen/a_fighteraxe.cpp \
	$(srcdir)/g_hexen/a_fighterboss.cpp \
	$(srcdir)/g_hexen/a_fighterhammer.cpp \
	$(srcdir)/g_hexen/a_fighterplayer.cpp \
	$(srcdir)/g_hexen/a_fighterquietus.cpp \
	$(srcdir)/g_hexen/a_firedemon.cpp \
	$(srcdir)/g_hexen/a_flame.cpp \
	$(srcdir)/g_hexen/a_flechette.cpp \
	$(srcdir)/g_hexen/a_fog.cpp \
	$(srcdir)/g_hexen/a_healingradius.cpp \
	$(srcdir)/g_hexen/a_heresiarch.cpp \
	$(srcdir)/g_hexen/a_hexenarmor.cpp \
	$(srcdir)/g_hexen/a_hexendecorations.cpp \
	$(srcdir)/g_hexen/a_hexenkeys.cpp \
	$(srcdir)/g_hexen/a_hexenspecialdecs.cpp \
	$(srcdir)/g_hexen/a_iceguy.cpp \
	$(srcdir)/g_hexen/a_korax.cpp \
	$(srcdir)/g_hexen/a_mageboss.cpp \
	$(srcdir)/g_hexen/a_magecone.cpp \
	$(srcdir)/g_hexen/a_magelightning.cpp \
	$(srcdir)/g_hexen/a_mageplayer.cpp \
	$(srcdir)/g_hexen/a_magestaff.cpp \
	$(srcdir)/g_hexen/a_magewand.cpp \
	$(srcdir)/g_hexen/a_mana.cpp \
	$(srcdir)/g_hexen/a_pig.cpp \
	$(srcdir)/g_hexen/a_puzzleitems.cpp \
	$(srcdir)/g_hexen/a_scriptprojectiles.cpp \
	$(srcdir)/g_hexen/a_serpent.cpp \
	$(srcdir)/g_hexen/a_speedboots.cpp \
	$(srcdir)/g_hexen/a_spike.cpp \
	$(srcdir)/g_hexen/a_summon.cpp \
	$(srcdir)/g_hexen/a_teleportother.cpp \
	$(srcdir)/g_hexen/a_weaponpieces.cpp \
	$(srcdir)/g_hexen/a_wraith.cpp \
	$(srcdir)/g_hexen/hexen_sbar.cpp \
	$(srcdir)/g_strife/a_acolyte.cpp \
	$(srcdir)/g_strife/a_alienspectres.cpp \
	$(srcdir)/g_strife/a_beggars.cpp \
	$(srcdir)/g_strife/a_coin.cpp \
	$(srcdir)/g_strife/a_entityboss.cpp \
	$(srcdir)/g_strife/a_loremaster.cpp \
	$(srcdir)/g_strife/a_macil.cpp \
	$(srcdir)/g_strife/a_merchants.cpp \
	$(srcdir)/g_strife/a_oracle.cpp \
	$(srcdir)/g_strife/a_peasant.cpp \
	$(srcdir)/g_strife/a_programmer.cpp \
	$(srcdir)/g_strife/a_questitems.cpp \
	$(srcdir)/g_strife/a_rebels.cpp \
	$(srcdir)/g_strife/a_sentinel.cpp \
	$(srcdir)/g_strife/a_spectral.cpp \
	$(srcdir)/g_strife/a_strifeammo.cpp \
	$(srcdir)/g_strife/a_strifeitems.cpp \
	$(srcdir)/g_strife/a_strifekeys.cpp \
	$(srcdir)/g_strife/a_strifeplayer.cpp \
	$(srcdir)/g_strife/a_strifestuff.cpp \
	$(srcdir)/g_strife/a_strifeweapons.cpp \
	$(srcdir)/g_strife/a_zombie.cpp \
	$(srcdir)/g_strife/strife_sbar.cpp \
	$(srcdir)/r_bsp.cpp \
	$(srcdir)/r_data.cpp \
	$(srcdir)/r_draw.cpp \
	$(srcdir)/r_drawt.cpp \
	$(srcdir)/r_main.cpp \
	$(srcdir)/r_plane.cpp \
	$(srcdir)/r_segs.cpp \
	$(srcdir)/r_sky.cpp \
	$(srcdir)/r_things.cpp \
	$(srcdir)/oplsynth/fmopl.cpp \
	$(srcdir)/oplsynth/mlkernel.cpp \
	$(srcdir)/oplsynth/mlopl.cpp \
	$(srcdir)/oplsynth/mlopl_io.cpp \
	$(srcdir)/oplsynth/opl_mus_player.cpp \
	$(srcdir)/sound/i_music.cpp \
	$(srcdir)/sound/i_sound.cpp \
	$(srcdir)/sound/music_cd.cpp \
	$(srcdir)/sound/music_flac.cpp \
	$(srcdir)/sound/music_midi_timidity.cpp \
	$(srcdir)/sound/music_mod.cpp \
	$(srcdir)/sound/music_mus_opl.cpp \
	$(srcdir)/sound/music_stream.cpp \
	$(srcdir)/sound/sample_flac.cpp \
	$(srcdir)/sdl/i_main.cpp \
	$(srcdir)/sdl/i_net.cpp \
	$(srcdir)/sdl/i_system.cpp \
	$(srcdir)/sdl/i_cd.cpp \
	$(srcdir)/sdl/i_input.cpp \
	$(srcdir)/sdl/i_movie.cpp \
	$(srcdir)/sdl/hardware.cpp \
	$(srcdir)/sdl/sdlvideo.cpp \
	$(srcdir)/sdl/i_altsound.cpp \
	$(srcdir)/autozend.cpp

SUFFIXES = .nas

.nas.o:
	$(NASM) @NASMFLAGS@ -o $@ $<
