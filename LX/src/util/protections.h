#pragma once

namespace protections
{
	bool is_crash_object(rage::joaat_t model);
	bool is_crash_ped(rage::joaat_t model);
	bool is_crash_vehicle(rage::joaat_t model);
	bool is_valid_player_model(rage::joaat_t model);
	bool is_cage_object(rage::joaat_t model);

	static const std::set<rage::joaat_t> crash_peds = {"slod_human"_joaat, "slod_small_quadped"_joaat, "slod_large_quadped"_joaat};
	static const std::set<rage::joaat_t> crash_vehicles = { "arbitergt"_joaat, "astron2"_joaat, "cyclone2"_joaat, "ignus2"_joaat, "s95"_joaat };
	static const std::set<rage::joaat_t> crash_objects = {
		"proc_drygrasses01"_joaat, "proc_drygrasses01b"_joaat, "proc_brittlebush_01"_joaat, "proc_forest_ivy_01"_joaat,
		"proc_grassdandelion01"_joaat, "proc_grasses01b"_joaat, "proc_grassfronds01"_joaat, "proc_grassplantmix_01"_joaat,
		"proc_indian_pbrush_01"_joaat, "proc_leafybush_01"_joaat, "proc_lizardtail_01"_joaat, "proc_lupins_01"_joaat,
		"proc_meadowmix_01"_joaat, "proc_meadowpoppy_01"_joaat, "proc_desert_sage_01"_joaat, "prop_saplin_001_b"_joaat,
		"prop_dummy_01"_joaat, "prop_dummy_car"_joaat, "prop_dummy_light"_joaat, "prop_dummy_plane"_joaat,
		"prop_distantcar_night"_joaat, "prop_distantcar_day"_joaat, "hei_bh1_08_details4_em_night"_joaat, "dt1_18_sq_night_slod"_joaat,
		"ss1_12_night_slod"_joaat, "hash_b334b5e2_qyquzxq_collision"_joaat, "h4_prop_bush_bgnvla_med_01"_joaat, "h4_prop_bush_bgnvla_lrg_01"_joaat,
		"h4_prop_bush_buddleia_low_01"_joaat, "h4_prop_bush_ear_aa"_joaat, "h4_prop_bush_ear_ab"_joaat, "h4_prop_bush_fern_low_01"_joaat,
		"h4_prop_bush_fern_tall_cc"_joaat, "h4_prop_bush_mang_ad"_joaat, "h4_prop_bush_mang_low_aa"_joaat, "h4_prop_bush_mang_low_ab"_joaat,
		"h4_prop_bush_seagrape_low_01"_joaat, "prop_h4_ground_cover"_joaat, "h4_prop_weed_groundcover_01"_joaat, "h4_prop_grass_med_01"_joaat,
		"h4_prop_grass_tropical_lush_01"_joaat, "h4_prop_grass_wiregrass_01"_joaat, "h4_prop_weed_01_plant"_joaat, "h4_prop_weed_01_row"_joaat,
		"urbanweeds02_l1"_joaat, "proc_forest_grass01"_joaat, "prop_small_bushyba"_joaat, "v_res_d_dildo_a"_joaat,
		"v_res_d_dildo_b"_joaat, "v_res_d_dildo_c"_joaat, "v_res_d_dildo_d"_joaat, "v_res_d_dildo_e"_joaat,
		"v_res_d_dildo_f"_joaat, "v_res_skateboard"_joaat, "prop_battery_01"_joaat, "prop_barbell_01"_joaat,
		"prop_barbell_02"_joaat, "prop_bandsaw_01"_joaat, "prop_bbq_3"_joaat, "v_med_curtainsnewcloth2"_joaat,
		"bh1_07_flagpoles"_joaat, "hash_058a7eb5_deihiws_collision"_joaat, "proc_dry_plants_01"_joaat, "proc_leafyplant_01"_joaat,
		"proc_grassplantmix_02"_joaat, "proc_dryplantsgrass_01"_joaat, "proc_dryplantsgrass_02"_joaat, "proc_grasses01"_joaat,
		"prop_dryweed_002_a"_joaat, "prop_fernba"_joaat, "prop_weed_001_aa"_joaat, "urbangrnfrnds_01"_joaat,
		"urbanweeds01"_joaat, "prop_dandy_b"_joaat, "v_proc2_temp"_joaat, "prop_fernbb"_joaat,
		"proc_drygrassfronds01"_joaat, "prop_log_ae"_joaat, "prop_grass_da"_joaat, "prop_fragtest_cnst_04"_joaat,
		"prop_gold_cont_01b"_joaat, "prop_gold_trolly"_joaat,
		"prop_gold_trolly_full"_joaat, "prop_gold_trolly_strap_01"_joaat, "prop_dog_cage_01"_joaat, "prop_dog_cage_02"_joaat,
		"prop_gascage01"_joaat, "prop_conslift_cage"_joaat, "P_CableCar_S"_joaat, "prop_ld_container"_joaat,
		"prop_rub_cont_01a"_joaat, "prop_rub_cont_01b"_joaat, "prop_rub_cont_01c"_joaat, "prop_cont_chiller_01"_joaat,
		"Prop_FncLink_03g"_joaat, "prop_container_01a"_joaat, "prop_container_01b"_joaat, "prop_container_01c"_joaat,
		"prop_container_01d"_joaat, "prop_container_01e"_joaat, "prop_container_01f"_joaat, "prop_container_01g"_joaat,
		"prop_container_01h"_joaat, "prop_container_01mb"_joaat, "prop_container_02a"_joaat, "prop_container_03a"_joaat,
		"prop_container_03b"_joaat, "prop_container_03mb"_joaat, "prop_container_03_ld"_joaat, "prop_container_04a"_joaat,
		"prop_container_04mb"_joaat, "prop_container_05mb"_joaat, "prop_container_door_mb_l"_joaat, "prop_container_door_mb_r"_joaat,
		"prop_container_hole"_joaat, "prop_container_ld"_joaat, "prop_container_ld2"_joaat, "prop_container_old1"_joaat,
		"prop_cctv_cont_01"_joaat, "prop_cctv_cont_03"_joaat, "prop_cctv_cont_04"_joaat, "prop_cctv_cont_05"_joaat,
		"prop_cctv_cont_06"_joaat, "prop_contnr_pile_01a"_joaat, "prop_controller_01"_joaat, "prop_control_rm_door_01"_joaat,
		"prop_facgate_05_r_dam_l1"_joaat, "v_61_lng_mesh_unita_swap"_joaat, "prop_crt_mon_02"_joaat, "xs_prop_arena_pit_fire_01a_wl"_joaat,
		"prop_billboard_07"_joaat, "prop_recyclebin_02_d"_joaat, "prop_billboard_14"_joaat, "_res_fa_book02"_joaat,
		"sf_int1_dropdownlight041"_joaat, "w_sg_pumpshotgunmk2_camo6"_joaat, "ch_prop_cash_low_trolly_01b"_joaat, "prop_ld_headset_01"_joaat,
		"sf_int1_office_wpaper_6"_joaat, "v_74_it3_ceiling_smoke_01_skin"_joaat, "prop_sh_tt_fridgedoor"_joaat, "ch_prop_arcade_race_02a"_joaat,
		"v_61_kit_over_dec_cruma"_joaat, "gr_prop_gr_rsply_crate01a"_joaat, "prop_snow_streetlight_01_frag_"_joaat, "v_34_sm_proc"_joaat,
		"tr_int1_lightcap_proxy001"_joaat, "v_ilev_tort_door"_joaat, "ch_prop_arcade_street_01b"_joaat, "prop_ic_repair_p"_joaat,
		"sf_int2_elevator_details_02"_joaat, "prop_cs4_05_tdoor"_joaat, "vfx_it1_02"_joaat, "cloudhat_wispy_b"_joaat,
		"hei_p_pre_heist_trash"_joaat, "v_ilev_trev_patiodoor"_joaat, "prop_bin_10a"_joaat, "sm_prop_offchair_smug_01"_joaat,
		"stt_prop_stunt_tube_fn_02"_joaat, "m23_2_prop_m32_hat_captain_01a"_joaat, "vw_prop_vw_arcade_04d_screen"_joaat, "prop_food_juice02"_joaat,
		"v_28_alrm_case011"_joaat, "apa_mp_h_acc_pot_pouri_01"_joaat, "xm3_prop_xm3_crate_ammo_01a"_joaat, "prop_blackjack_01"_joaat,
		"prop_cs_mini_tv"_joaat, "v_res_fa_basket"_joaat, "prop_plant_int_02b"_joaat, "prop_umpire_01"_joaat,
		"gr_prop_gr_fnclink_03g"_joaat, "prop_rub_tyre_dam3"_joaat, "ba_prop_battle_lights_support"_joaat, "prop_fnclink_09a"_joaat,
		"ba_prop_battle_dj_mixer_01e"_joaat, "ar_prop_ar_neon_gate8x_04a"_joaat, "xs_propintarena_structure_f_03b"_joaat, "des_plog_door_end"_joaat,
		"v_ret_fh_chair01"_joaat, "gr_prop_gr_offchair_01a"_joaat, "hei_p_pre_heist_coke"_joaat, "cloudhat_puff_b"_joaat,
		"v_ind_cfbucket"_joaat, "p_hw1_22_doors_s"_joaat, "arbitergt"_joaat, "slod_human"_joaat,
		"cablecar"_joaat, "prop_beach_fire"_joaat, "prop_feeder1_cr"_joaat, "bkr_prop_biker_tube_crn"_joaat,
		"bkr_prop_biker_tube_crn2"_joaat, "bkr_prop_biker_tube_gap_01"_joaat, "bkr_prop_biker_tube_gap_02"_joaat, "bkr_prop_biker_tube_l"_joaat,
		"bkr_prop_biker_tube_m"_joaat, "bkr_prop_biker_tube_s"_joaat, "bkr_prop_biker_tube_xs"_joaat, "bkr_prop_biker_tube_xxs"_joaat,
		"prop_rub_cage01c"_joaat, "prop_conslift_lift"_joaat, "Prop_Portaloo_01A"_joaat, "gr_prop_gr_cnc_01a"_joaat,
		"luxor"_joaat, "prop_rub_cage01b"_joaat, "Prop_TankTrailer_01A"_joaat, "Prop_Aircon_M_01"_joaat,
		"h4_prop_h4_safe_01a"_joaat, "hash_92962485_eykqyul_collision"_joaat, "prop_thindesertfiller_aa"_joaat /*, static_cast<uint32_t>(-1288391198)*/
	};

	static const std::set<rage::joaat_t> valid_player_models = {
		"mp_m_freemode_01"_joaat, "mp_f_freemode_01"_joaat, "u_m_m_filmdirector"_joaat, "player_zero"_joaat,
		"player_one"_joaat, "player_two"_joaat, 
		// peyote
		"A_C_Boar"_joaat, "A_C_Cat_01"_joaat, "A_C_Cow"_joaat, "A_C_Coyote"_joaat, "A_C_Deer"_joaat,
		"A_C_Husky"_joaat, "A_C_MtLion"_joaat, "A_C_Pig"_joaat, "A_C_Poodle"_joaat, "A_C_Pug"_joaat,
		"A_C_Rabbit_01"_joaat, "A_C_Retriever"_joaat, "A_C_Rottweiler"_joaat, "A_C_shepherd"_joaat,
		"A_C_Westy"_joaat, "A_C_Chickenhawk"_joaat, "A_C_Cormorant"_joaat, "A_C_Crow"_joaat,
		"A_C_Hen"_joaat, "A_C_Pigeon"_joaat, "A_C_Seagull"_joaat, "A_C_Dolphin"_joaat,
		"A_C_Fish"_joaat, "A_C_KillerWhale"_joaat, "A_C_SharkHammer"_joaat, "A_C_SharkTiger"_joaat,
		"A_C_Stingray"_joaat, "IG_Orleans"_joaat, "A_C_Chop"_joaat, "A_C_HumpBack"_joaat,
	};

	static const std::set<rage::joaat_t> cage_objects = { "stt_prop_stunt_tube_s"_joaat, "prop_fnclink_03e"_joaat, "prop_gold_cont_01"_joaat, "prop_gold_cont_01b"_joaat, "prop_rub_cage01a"_joaat };
}
