define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//event_filters', 'collection//sensors',
//localize
'i18n!strings/nls/event_filters',
//template
'text!templates/settings/pef/event_filters_edit_item.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, EventFiltersCollection, SensorsCollection, locale, EventFiltersEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(EventFiltersEditItemTemplate),

        initialize: function() {
            EventFiltersCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = EventFiltersCollection.get(id);
            } else {
                this.model = new EventFiltersCollection.model;
                this.model.url = EventFiltersCollection.url;
            };
            this.model.bind('change:enable_filter', this.updateEnableFilter, this);
            this.model.bind('change:trigger_event_severity', this.updateTriggerEventSeverity, this);
            this.model.bind('change:power_action', this.updatePowerAction, this);
            this.model.bind('change:policy_group', this.updatePolicyGroup, this);
            this.model.bind('change:raw_data', this.updateRawData, this);
            this.model.bind('change:generator_id_1', this.updateGeneratorId1, this);
            this.model.bind('change:generator_id_2', this.updateGeneratorId2, this);
            this.model.bind('change:generator_type', this.updateGeneratorType, this);
            this.model.bind('change:slave_addr_software_id', this.updateSlaveAddrSoftwareId, this);
            this.model.bind('change:channel_number', this.updateChannelNumber, this);
            this.model.bind('change:ipmb_lun', this.updateIpmbLun, this);
            this.model.bind('change:sensor_type', this.updateSensorType, this);
            this.model.bind('change:name', this.updateName, this);
            this.model.bind('change:sensor_num', this.updateSensorNum, this);
            this.model.bind('change:event_options', this.updateEventOptions, this);
            this.model.bind('change:event_trigger', this.updateEventTrigger, this);
            this.model.bind('change:event_data_1_offset_mask', this.updateEventData1OffsetMask, this);
            this.model.bind('change:event_data_1_and_mask', this.updateEventData1AndMask, this);
            this.model.bind('change:event_data_1_compare_1', this.updateEventData1Compare1, this);
            this.model.bind('change:event_data_1_compare_2', this.updateEventData1Compare2, this);
            this.model.bind('change:event_data_2_and_mask', this.updateEventData2AndMask, this);
            this.model.bind('change:event_data_2_compare_1', this.updateEventData2Compare1, this);
            this.model.bind('change:event_data_2_compare_2', this.updateEventData2Compare2, this);
            this.model.bind('change:event_data_3_and_mask', this.updateEventData3AndMask, this);
            this.model.bind('change:event_data_3_compare_1', this.updateEventData3Compare1, this);
            this.model.bind('change:event_data_3_compare_2', this.updateEventData3Compare2, this);
            this.sensors = SensorsCollection;
            this.sensors.bind('add', this.addSensors, this);
            
            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "ifChecked #idenable_filter": "checkedEnableFilter",
            "ifUnchecked #idenable_filter": "uncheckedEnableFilter",
            "ifChecked #idraw_data": "checkedRawData",
            "ifUnchecked #idraw_data": "uncheckedRawData",
            "ifChecked input[name='generator_type']": "checkedGeneratorType",
            "ifUnchecked input[name='generator_type']": "uncheckedGeneratorType",
            "click #delete": "deleteConfirm",
            "click #save": "save",
            "click .help-link": "toggleHelp",
            "change #idsensor_type": "sensorTypeChanged",
            "change #idslave_addr_software_id": "changeGenId1",
            "change #idchannel_number": "changeGenId2",
            "change #idipmb_lun": "changeGenId2",
            "change #idevent_options": "displaySensorEvents",
            "change #idname": "sensorNameChanged"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.createSensorSpecificStrings.call(this);
            this.all_sensors = [];
            this.sensor_val = "";
            this.$el.find('#idenable_filter,#idraw_data,input[name="generator_type"]').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            $('#idname').append($('<option></option>').attr('value', 255).text("All Sensors"));
            this.sensors.fetch();

            //this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, this.model, value);
            });

            this.sensors.each(function(model) {
                this.addSensors(model, this.sensors);
            }, this);

            if($("#sensor_events").is(":visible")) {
                $("#idevent_options option[value='sensor']").prop('selected', true);
            }
        },

        load: function(model, collection, xhr) {
            
        },

        reload: function(model, value) {

        },

        createSensorSpecificStrings: function() {
            this.sensor_specific_event_strings = new Array();

            // Threshold sensors
            this.sensor_specific_event_strings[0] = locale.event_filtersreserved;
            this.sensor_specific_event_strings[1] = locale.event_filterstemperature;
            this.sensor_specific_event_strings[2] = locale.event_filtersvoltage;
            this.sensor_specific_event_strings[3] = locale.event_filterscurrent;
            this.sensor_specific_event_strings[4] = locale.event_filtersfan;

            // Chassis intrusion
            this.sensor_specific_event_strings[5] = new Array();
            this.sensor_specific_event_strings[5][0] = locale.event_filtersgeneral_chassis_intrusion;
            this.sensor_specific_event_strings[5][1] = locale.event_filtersdrive_bay_intrusion;
            this.sensor_specific_event_strings[5][2] = locale.event_filtersio_card_area_intrusion;
            this.sensor_specific_event_strings[5][3] = locale.event_filtersprocessor_area_intrusion;
            this.sensor_specific_event_strings[5][4] = locale.event_filterslan_leash_lost_system_unplugged_from_lan;
            this.sensor_specific_event_strings[5][5] = locale.event_filtersunauthorized_dock;
            this.sensor_specific_event_strings[5][6] = locale.event_filtersfan_area_intrusion;

            // Platform security violation
            this.sensor_specific_event_strings[6] = new Array();
            this.sensor_specific_event_strings[6][0] = locale.event_filterssecure_mode_violation_attempt;
            this.sensor_specific_event_strings[6][1] = locale.event_filterspreboot_password_violation__user_password;
            this.sensor_specific_event_strings[6][2] = locale.event_filterspreboot_password_violation_attempt__setup_password;
            this.sensor_specific_event_strings[6][3] = locale.event_filterspreboot_password_violation__network_boot_password;
            this.sensor_specific_event_strings[6][4] = locale.event_filtersother_preboot_password_violation;
            this.sensor_specific_event_strings[6][5] = locale.event_filtersoutofband_access_password_violation;

            // Processor
            this.sensor_specific_event_strings[7] = new Array();
            this.sensor_specific_event_strings[7][0] = locale.event_filtersierr;
            this.sensor_specific_event_strings[7][1] = locale.event_filtersthermal_trip;
            this.sensor_specific_event_strings[7][2] = locale.event_filtersfrb1bist_failure;
            this.sensor_specific_event_strings[7][3] = locale.event_filtersfrb2hang_in_post_faliure;
            this.sensor_specific_event_strings[7][4] = locale.event_filtersfrb3processor_startupinitialization_failure;
            this.sensor_specific_event_strings[7][5] = locale.event_filtersconfiguration_error_dmi;
            this.sensor_specific_event_strings[7][6] = locale.event_filterssm_bios_uncorrectable_cpucomplex_error;
            this.sensor_specific_event_strings[7][7] = locale.event_filtersprocessor_presence_detected;
            this.sensor_specific_event_strings[7][8] = locale.event_filtersprocessor_disabled;
            this.sensor_specific_event_strings[7][9] = locale.event_filtersterminator_presence_detected;
            this.sensor_specific_event_strings[7][10] = locale.event_filtersprocessor_automatically_throttled;
            this.sensor_specific_event_strings[7][11] = locale.event_filtersmachine_check_exception;
            this.sensor_specific_event_strings[7][12] = locale.event_filterscorrectable_machine_check_error;

            // Power supply
            this.sensor_specific_event_strings[8] = new Array();
            this.sensor_specific_event_strings[8][0] = locale.event_filterspresence_detected;
            this.sensor_specific_event_strings[8][1] = locale.event_filterspower_supply_failure_detected;
            this.sensor_specific_event_strings[8][2] = locale.event_filterspredictive_failure_asserted;
            this.sensor_specific_event_strings[8][3] = locale.event_filterspower_supply_input_lost_acdc;
            this.sensor_specific_event_strings[8][4] = locale.event_filterspower_supply_input_lost_or_out_of_range;
            this.sensor_specific_event_strings[8][5] = locale.event_filterspower_supply_input_out_of_range_but_present;
            this.sensor_specific_event_strings[8][6] = locale.event_filtersconfiguration_error;

            // Power unit
            this.sensor_specific_event_strings[9] = new Array();
            this.sensor_specific_event_strings[9][0] = locale.event_filterspower_off__power_down;
            this.sensor_specific_event_strings[9][1] = locale.event_filterspower_cycle;
            this.sensor_specific_event_strings[9][2] = locale.event_filters240va_power_down;
            this.sensor_specific_event_strings[9][3] = locale.event_filtersinterlock_power_down;
            this.sensor_specific_event_strings[9][4] = locale.event_filtersac_lost__power_input_lost;
            this.sensor_specific_event_strings[9][5] = locale.event_filterssoft_power_control_failure;
            this.sensor_specific_event_strings[9][6] = locale.event_filterspower_unit_failure_detected;
            this.sensor_specific_event_strings[9][7] = locale.event_filterspredictive_failure;

            // Memory
            this.sensor_specific_event_strings[12] = new Array();
            this.sensor_specific_event_strings[12][0] = locale.event_filterscorrectable_ecc;
            this.sensor_specific_event_strings[12][1] = locale.event_filtersuncorrectable_ecc;
            this.sensor_specific_event_strings[12][2] = locale.event_filtersparity;
            this.sensor_specific_event_strings[12][3] = locale.event_filtersmemory_scrub_failure;
            this.sensor_specific_event_strings[12][4] = locale.event_filtersmemory_device_disabled;
            this.sensor_specific_event_strings[12][5] = locale.event_filterscorrectable_ecc_logging_limit_reached;
            this.sensor_specific_event_strings[12][6] = locale.event_filterspresence_detected;
            this.sensor_specific_event_strings[12][7] = locale.event_filtersconfiguration_error;
            this.sensor_specific_event_strings[12][8] = locale.event_filtersspare;
            this.sensor_specific_event_strings[12][9] = locale.event_filtersmemory_automatically_throttled;
            this.sensor_specific_event_strings[12][10] = locale.event_filterscritical_overtemperature;

            // Drive slot
            this.sensor_specific_event_strings[13] = new Array();
            this.sensor_specific_event_strings[13][0] = locale.event_filtersdrive_presence;
            this.sensor_specific_event_strings[13][1] = locale.event_filtersdrive_fault;
            this.sensor_specific_event_strings[13][2] = locale.event_filterspredictive_failure;
            this.sensor_specific_event_strings[13][3] = locale.event_filtershot_spare;
            this.sensor_specific_event_strings[13][4] = locale.event_filtersconsistency_check__parity_check_in_progress;
            this.sensor_specific_event_strings[13][5] = locale.event_filtersin_critical_array;
            this.sensor_specific_event_strings[13][6] = locale.event_filtersin_failed_array;
            this.sensor_specific_event_strings[13][7] = locale.event_filtersrebuildremap_in_progress;
            this.sensor_specific_event_strings[13][8] = locale.event_filtersrebuildremap_aborted;

            // System firmware error
            this.sensor_specific_event_strings[15] = new Array();
            this.sensor_specific_event_strings[15][0] = locale.event_filterserror;
            this.sensor_specific_event_strings[15][1] = locale.event_filtershang;
            this.sensor_specific_event_strings[15][2] = locale.event_filtersprogress;

            // Event logging disabled
            this.sensor_specific_event_strings[16] = new Array();
            this.sensor_specific_event_strings[16][0] = locale.event_filterscorrectable_memory_error_logging_disabled;
            this.sensor_specific_event_strings[16][1] = locale.event_filtersevent_type_logging_disabled;
            this.sensor_specific_event_strings[16][2] = locale.event_filterslog_area_resetcleared;
            this.sensor_specific_event_strings[16][3] = locale.event_filtersall_event_logging_disabled;
            this.sensor_specific_event_strings[16][4] = locale.event_filterssel_full;
            this.sensor_specific_event_strings[16][5] = locale.event_filterssel_almost_full;
            this.sensor_specific_event_strings[16][6] = locale.event_filterscorrectable_machine_check_error_logging_disabled;

            // Watchdog
            this.sensor_specific_event_strings[17] = new Array();
            this.sensor_specific_event_strings[17][0] = locale.event_filtersbios_watchdog_reset;
            this.sensor_specific_event_strings[17][1] = locale.event_filtersos_watchdog_reset;
            this.sensor_specific_event_strings[17][2] = locale.event_filtersos_watchdog_shutdown;
            this.sensor_specific_event_strings[17][3] = locale.event_filtersos_watchdog_power_down;
            this.sensor_specific_event_strings[17][4] = locale.event_filtersos_watchdog_power_cycle;
            this.sensor_specific_event_strings[17][5] = locale.event_filtersos_watchdog_nmi;
            this.sensor_specific_event_strings[17][6] = locale.event_filtersos_watchdog_expired;
            this.sensor_specific_event_strings[17][7] = locale.event_filtersos_watchdog_pretimeout_interrupt_nonnmi;

            // System event
            this.sensor_specific_event_strings[18] = new Array();
            this.sensor_specific_event_strings[18][0] = locale.event_filterssystem_reconfigured;
            this.sensor_specific_event_strings[18][1] = locale.event_filtersoem_system_boot_event;
            this.sensor_specific_event_strings[18][2] = locale.event_filtersundetermined_system_hardware_failure;
            this.sensor_specific_event_strings[18][3] = locale.event_filtersentry_added_to_auxiliary_log;
            this.sensor_specific_event_strings[18][4] = locale.event_filterspef_action;
            this.sensor_specific_event_strings[18][5] = locale.event_filterstimestamp_clock_synch;

            // Critical interrupt
            this.sensor_specific_event_strings[19] = new Array();
            this.sensor_specific_event_strings[19][0] = locale.event_filtersfront_panel_nmi;
            this.sensor_specific_event_strings[19][1] = locale.event_filtersbus_timeout;
            this.sensor_specific_event_strings[19][2] = locale.event_filtersio_channel_check_nmi;
            this.sensor_specific_event_strings[19][3] = locale.event_filterssoftware_nmi;
            this.sensor_specific_event_strings[19][4] = locale.event_filterspci_perr;
            this.sensor_specific_event_strings[19][5] = locale.event_filterspci_serr;
            this.sensor_specific_event_strings[19][6] = locale.event_filterseisa_fail_safe_timeout;
            this.sensor_specific_event_strings[19][7] = locale.event_filtersbus_correctable_error;
            this.sensor_specific_event_strings[19][8] = locale.event_filtersbus_uncorrectable_error;
            this.sensor_specific_event_strings[19][9] = locale.event_filtersfatal_nmi;
            this.sensor_specific_event_strings[19][10] = locale.event_filtersbus_fatal_error;
            this.sensor_specific_event_strings[19][11] = locale.event_filtersbus_degraded;

            // Button
            this.sensor_specific_event_strings[20] = new Array();
            this.sensor_specific_event_strings[20][0] = locale.event_filterspower_button_pressed;
            this.sensor_specific_event_strings[20][1] = locale.event_filterssleep_button_pressed;
            this.sensor_specific_event_strings[20][2] = locale.event_filtersreset_button_pressed;
            this.sensor_specific_event_strings[20][3] = locale.event_filtersfru_latch_open;
            this.sensor_specific_event_strings[20][4] = locale.event_filtersfru_service_request_button;

            // Chipset
            this.sensor_specific_event_strings[25] = new Array();
            this.sensor_specific_event_strings[25][0] = locale.event_filterssoft_power_control_failure;
            this.sensor_specific_event_strings[25][1] = locale.event_filtersthermal_trip;

            // Cable
            this.sensor_specific_event_strings[27] = new Array();
            this.sensor_specific_event_strings[27][0] = locale.event_filterscableinterconnect_is_connected;
            this.sensor_specific_event_strings[27][1] = locale.event_filtersconfiguration_error;

            // System boot
            this.sensor_specific_event_strings[29] = new Array();
            this.sensor_specific_event_strings[29][0] = locale.event_filtersinitiated_by_power_up;
            this.sensor_specific_event_strings[29][1] = locale.event_filtersinitiated_by_hard_reset;
            this.sensor_specific_event_strings[29][2] = locale.event_filtersinitiated_by_warm_reset;
            this.sensor_specific_event_strings[29][3] = locale.event_filtersuser_requested_pxe_boot;
            this.sensor_specific_event_strings[29][4] = locale.event_filtersautomatic_boot_to_diagnostic;
            this.sensor_specific_event_strings[29][5] = locale.event_filtersos__runtime_software_initiated_hard_reset;
            this.sensor_specific_event_strings[29][6] = locale.event_filtersos__runtime_software_initiated_warm_reset;
            this.sensor_specific_event_strings[29][7] = locale.event_filterssystem_restart;

            // Boot error
            this.sensor_specific_event_strings[30] = new Array();
            this.sensor_specific_event_strings[30][0] = locale.event_filtersno_bootable_media;
            this.sensor_specific_event_strings[30][1] = locale.event_filtersnonbootable_diskette_left_in_drive;
            this.sensor_specific_event_strings[30][2] = locale.event_filterspxe_server_not_found;
            this.sensor_specific_event_strings[30][3] = locale.event_filtersinvalid_boot_sector;
            this.sensor_specific_event_strings[30][4] = locale.event_filterstimeout_waiting_for_user_selection_of_boot_source;

            // Base OS boot
            this.sensor_specific_event_strings[31] = new Array();
            this.sensor_specific_event_strings[31][0] = locale.event_filtersa_boot_completed;
            this.sensor_specific_event_strings[31][1] = locale.event_filtersc_boot_completed;
            this.sensor_specific_event_strings[31][2] = locale.event_filterspxe_boot_completed;
            this.sensor_specific_event_strings[31][3] = locale.event_filtersdiagnostic_boot_completed;
            this.sensor_specific_event_strings[31][4] = locale.event_filterscdrom_boot_completed;
            this.sensor_specific_event_strings[31][5] = locale.event_filtersrom_boot_completed;
            this.sensor_specific_event_strings[31][6] = locale.event_filtersboot_completed__boot_device_not_specified;

            //OS shutdown
            this.sensor_specific_event_strings[32] = new Array();
            this.sensor_specific_event_strings[32][0] = locale.event_filterscritical_stop_during_os_load__initialization;
            this.sensor_specific_event_strings[32][1] = locale.event_filtersruntime_critical_stop;
            this.sensor_specific_event_strings[32][2] = locale.event_filtersos_graceful_stop;
            this.sensor_specific_event_strings[32][3] = locale.event_filtersos_graceful_shutdown;
            this.sensor_specific_event_strings[32][4] = locale.event_filterssoft_shutdown_initiated_by_pef;
            this.sensor_specific_event_strings[32][5] = locale.event_filtersagent_not_responding;

            // Slot
            this.sensor_specific_event_strings[33] = new Array();
            this.sensor_specific_event_strings[33][0] = locale.event_filtersfault_status_asserted;
            this.sensor_specific_event_strings[33][1] = locale.event_filtersidentify_status_asserted;
            this.sensor_specific_event_strings[33][2] = locale.event_filtersslotconnector_device_installedattached;
            this.sensor_specific_event_strings[33][3] = locale.event_filtersslotconnector_ready_for_device_installation;
            this.sensor_specific_event_strings[33][4] = locale.event_filtersslotconnector_ready_for_device_removal;
            this.sensor_specific_event_strings[33][5] = locale.event_filtersslot_power_is_off;
            this.sensor_specific_event_strings[33][6] = locale.event_filtersslotconnector_device_removal_request;
            this.sensor_specific_event_strings[33][7] = locale.event_filtersinterlock_asserted;
            this.sensor_specific_event_strings[33][8] = locale.event_filtersslot_is_disabled;
            this.sensor_specific_event_strings[33][9] = locale.event_filtersslot_holds_spare_device;

            // System ACPI
            this.sensor_specific_event_strings[34] = new Array();
            this.sensor_specific_event_strings[34][0] = locale.event_filterss0g0_working;
            this.sensor_specific_event_strings[34][1] = locale.event_filterss1_sleeping_with_system_hw__processor_context_maintained;
            this.sensor_specific_event_strings[34][2] = locale.event_filterss2_sleeping_processor_context_lost;
            this.sensor_specific_event_strings[34][3] = locale.event_filterss3_sleeping_processor__hw_context_lost_memory_retained;
            this.sensor_specific_event_strings[34][4] = locale.event_filterss4_nonvolatile_sleepsuspend_to_disk;
            this.sensor_specific_event_strings[34][5] = locale.event_filterss5g2_soft_off;
            this.sensor_specific_event_strings[34][6] = locale.event_filterss4s5_soft_off_particular_s4s5_state_cannot_be_determined;
            this.sensor_specific_event_strings[34][7] = locale.event_filtersg3__mechanical_off;
            this.sensor_specific_event_strings[34][8] = locale.event_filterssleeping_in_s1_s2_or_s3;
            this.sensor_specific_event_strings[34][9] = locale.event_filtersg1_sleeping;
            this.sensor_specific_event_strings[34][10] = locale.event_filterss5_entered_by_override;
            this.sensor_specific_event_strings[34][11] = locale.event_filterslegacy_on_state;
            this.sensor_specific_event_strings[34][12] = locale.event_filterslegacy_off_state;
            this.sensor_specific_event_strings[34][13] = locale.event_filtersunknown;

            // Watchdog
            this.sensor_specific_event_strings[35] = new Array();
            this.sensor_specific_event_strings[35][0] = locale.event_filterstimer_expired;
            this.sensor_specific_event_strings[35][1] = locale.event_filtershard_reset;
            this.sensor_specific_event_strings[35][2] = locale.event_filterspower_down;
            this.sensor_specific_event_strings[35][3] = locale.event_filterspower_cycle;
            this.sensor_specific_event_strings[35][4] = locale.event_filtersreserved;
            this.sensor_specific_event_strings[35][5] = locale.event_filtersreserved;
            this.sensor_specific_event_strings[35][6] = locale.event_filtersreserved;
            this.sensor_specific_event_strings[35][7] = locale.event_filtersreserved;
            this.sensor_specific_event_strings[35][8] = locale.event_filterstimer_interrupt;

            // Platform alert
            this.sensor_specific_event_strings[36] = new Array();
            this.sensor_specific_event_strings[36][0] = locale.event_filtersplatform_generated_page;
            this.sensor_specific_event_strings[36][1] = locale.event_filtersplatform_generated_lan_alert;
            this.sensor_specific_event_strings[36][2] = locale.event_filtersplatform_event_trap_generated;
            this.sensor_specific_event_strings[36][3] = locale.event_filtersplatform_generated_snmp_trap_oem_format;

            // Entity presence
            this.sensor_specific_event_strings[37] = new Array();
            this.sensor_specific_event_strings[37][0] = locale.event_filtersentity_present;
            this.sensor_specific_event_strings[37][1] = locale.event_filtersentity_absent;
            this.sensor_specific_event_strings[37][2] = locale.event_filtersentity_disabled;

            //LAN
            this.sensor_specific_event_strings[39] = new Array();
            this.sensor_specific_event_strings[39][0] = locale.event_filterslan_heartbeat_lost;
            this.sensor_specific_event_strings[39][1] = locale.event_filterslan_heartbeat;

            // Management subsystem health
            this.sensor_specific_event_strings[40] = new Array();
            this.sensor_specific_event_strings[40][0] = locale.event_filterssensor_access_degraded_or_unavailable;
            this.sensor_specific_event_strings[40][1] = locale.event_filterscontroller_access_degraded_or_unavailable;
            this.sensor_specific_event_strings[40][2] = locale.event_filtersmanagement_controller_offline;
            this.sensor_specific_event_strings[40][3] = locale.event_filtersmanagement_controller_unavailable;
            this.sensor_specific_event_strings[40][4] = locale.event_filterssensor_failure;
            this.sensor_specific_event_strings[40][5] = locale.event_filtersfru_failure;

            // Battery
            this.sensor_specific_event_strings[41] = new Array();
            this.sensor_specific_event_strings[41][0] = locale.event_filtersbattery_low;
            this.sensor_specific_event_strings[41][1] = locale.event_filtersbattery_failed;
            this.sensor_specific_event_strings[41][2] = locale.event_filtersbattery_presence_detected;

            // Session audit
            this.sensor_specific_event_strings[42] = new Array();
            this.sensor_specific_event_strings[42][0] = locale.event_filterssession_activated;
            this.sensor_specific_event_strings[42][1] = locale.event_filterssession_deactivated;
            this.sensor_specific_event_strings[42][2] = locale.event_filtersinvalid_username_or_password;
            this.sensor_specific_event_strings[42][3] = locale.event_filtersinvalid_password_disable;

            // Version change
            this.sensor_specific_event_strings[43] = new Array();
            this.sensor_specific_event_strings[43][0] = locale.event_filtershardware_change_detected;
            this.sensor_specific_event_strings[43][1] = locale.event_filtersfirmware__software_change_detected;
            this.sensor_specific_event_strings[43][2] = locale.event_filtershardware_incompatibility_detected;
            this.sensor_specific_event_strings[43][3] = locale.event_filtersfirmware__software_incompatibility_detected;
            this.sensor_specific_event_strings[43][4] = locale.event_filtersinvalid_or_unsupported_hardware_version;
            this.sensor_specific_event_strings[43][5] = locale.event_filtersinvalid_or_unsupported_firmware__software_version;
            this.sensor_specific_event_strings[43][6] = locale.event_filtershardware_change_detected_was_successful;
            this.sensor_specific_event_strings[43][7] = locale.event_filtersfirmware__software_change_detected_was_successful;

            // FRU state
            this.sensor_specific_event_strings[44] = new Array();
            this.sensor_specific_event_strings[44][0] = locale.event_filtersfru_not_installed;
            this.sensor_specific_event_strings[44][1] = locale.event_filtersfru_inactive;
            this.sensor_specific_event_strings[44][2] = locale.event_filtersfru_activation_requested;
            this.sensor_specific_event_strings[44][3] = locale.event_filtersfru_activation_in_progress;
            this.sensor_specific_event_strings[44][4] = locale.event_filtersfru_active;
            this.sensor_specific_event_strings[44][5] = locale.event_filtersfru_deactivation_requested;
            this.sensor_specific_event_strings[44][6] = locale.event_filtersfru_deactivation_in_progress;
            this.sensor_specific_event_strings[44][7] = locale.event_filtersfru_communication_lost;

            this.sensor_specific_event_strings[240] = new Array();
            this.sensor_specific_event_strings[240][0] = locale.event_filterstransition_to_m0;
            this.sensor_specific_event_strings[240][1] = locale.event_filterstransition_to_m1;
            this.sensor_specific_event_strings[240][2] = locale.event_filterstransition_to_m2;
            this.sensor_specific_event_strings[240][3] = locale.event_filterstransition_to_m3;
            this.sensor_specific_event_strings[240][4] = locale.event_filterstransition_to_m4;
            this.sensor_specific_event_strings[240][5] = locale.event_filterstransition_to_m5;
            this.sensor_specific_event_strings[240][6] = locale.event_filterstransition_to_m6;
            this.sensor_specific_event_strings[240][7] = locale.event_filterstransition_to_m7;

            this.sensor_specific_event_strings[241] = new Array();
            this.sensor_specific_event_strings[241][0] = locale.event_filtersipmba_disabled_ipmbb_disabled;
            this.sensor_specific_event_strings[241][1] = locale.event_filtersipmba_enabled_ipmbb_disabled;
            this.sensor_specific_event_strings[241][2] = locale.event_filtersipmba_disabled_ipmbb_enabled;
            this.sensor_specific_event_strings[241][3] = locale.event_filtersipmba_enabled_ipmpb_enabled;

            this.sensor_specific_event_strings[242] = new Array();
            this.sensor_specific_event_strings[242][0] = locale.event_filtersmodule_handle_closed;
            this.sensor_specific_event_strings[242][1] = locale.event_filtersmodule_handle_opened;
            this.sensor_specific_event_strings[242][2] = locale.event_filtersquiesced;

            this.sensor_specific_event_strings[192] = new Array();
            this.sensor_specific_event_strings[192][0] = locale.event_filtersoem_specific;


            this.sensor_specific_event_strings[193] = new Array();
            this.sensor_specific_event_strings[193][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[194] = new Array();
            this.sensor_specific_event_strings[194][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[195] = new Array();
            this.sensor_specific_event_strings[195][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[196] = new Array();
            this.sensor_specific_event_strings[196][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[197] = new Array();
            this.sensor_specific_event_strings[197][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[198] = new Array();
            this.sensor_specific_event_strings[198][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[199] = new Array();
            this.sensor_specific_event_strings[199][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[200] = new Array();
            this.sensor_specific_event_strings[200][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[201] = new Array();
            this.sensor_specific_event_strings[201][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[202] = new Array();
            this.sensor_specific_event_strings[202][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[203] = new Array();
            this.sensor_specific_event_strings[203][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[204] = new Array();
            this.sensor_specific_event_strings[204][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[205] = new Array();
            this.sensor_specific_event_strings[205][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[206] = new Array();
            this.sensor_specific_event_strings[206][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[207] = new Array();
            this.sensor_specific_event_strings[207][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[208] = new Array();
            this.sensor_specific_event_strings[208][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[209] = new Array();
            this.sensor_specific_event_strings[209][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[210] = new Array();
            this.sensor_specific_event_strings[210][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[211] = new Array();
            this.sensor_specific_event_strings[211][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[212] = new Array();
            this.sensor_specific_event_strings[212][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[213] = new Array();
            this.sensor_specific_event_strings[213][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[214] = new Array();
            this.sensor_specific_event_strings[214][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[215] = new Array();
            this.sensor_specific_event_strings[215][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[216] = new Array();
            this.sensor_specific_event_strings[216][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[217] = new Array();
            this.sensor_specific_event_strings[217][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[218] = new Array();
            this.sensor_specific_event_strings[218][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[219] = new Array();
            this.sensor_specific_event_strings[219][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[220] = new Array();
            this.sensor_specific_event_strings[220][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[221] = new Array();
            this.sensor_specific_event_strings[221][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[222] = new Array();
            this.sensor_specific_event_strings[222][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[223] = new Array();
            this.sensor_specific_event_strings[223][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[224] = new Array();
            this.sensor_specific_event_strings[224][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[225] = new Array();
            this.sensor_specific_event_strings[225][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[226] = new Array();
            this.sensor_specific_event_strings[226][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[227] = new Array();
            this.sensor_specific_event_strings[227][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[228] = new Array();
            this.sensor_specific_event_strings[228][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[229] = new Array();
            this.sensor_specific_event_strings[229][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[230] = new Array();
            this.sensor_specific_event_strings[230][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[231] = new Array();
            this.sensor_specific_event_strings[231][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[232] = new Array();
            this.sensor_specific_event_strings[232][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[233] = new Array();
            this.sensor_specific_event_strings[233][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[234] = new Array();
            this.sensor_specific_event_strings[234][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[235] = new Array();
            this.sensor_specific_event_strings[235][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[236] = new Array();
            this.sensor_specific_event_strings[236][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[237] = new Array();
            this.sensor_specific_event_strings[237][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[238] = new Array();
            this.sensor_specific_event_strings[238][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[239] = new Array();
            this.sensor_specific_event_strings[239][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[240] = new Array();
            this.sensor_specific_event_strings[240][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[241] = new Array();
            this.sensor_specific_event_strings[241][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[242] = new Array();
            this.sensor_specific_event_strings[242][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[243] = new Array();
            this.sensor_specific_event_strings[243][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[244] = new Array();
            this.sensor_specific_event_strings[244][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[245] = new Array();
            this.sensor_specific_event_strings[245][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[246] = new Array();
            this.sensor_specific_event_strings[246][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[247] = new Array();
            this.sensor_specific_event_strings[247][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[248] = new Array();
            this.sensor_specific_event_strings[248][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[249] = new Array();
            this.sensor_specific_event_strings[249][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[250] = new Array();
            this.sensor_specific_event_strings[250][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[251] = new Array();
            this.sensor_specific_event_strings[251][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[252] = new Array();
            this.sensor_specific_event_strings[252][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[253] = new Array();
            this.sensor_specific_event_strings[253][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[254] = new Array();
            this.sensor_specific_event_strings[254][0] =      locale.event_filtersoem_specific;
            this.sensor_specific_event_strings[255] = new Array();
            this.sensor_specific_event_strings[255][0] =      locale.event_filtersoem_specific;

        },

        sensorNameChanged: function() {
            $("#idevent_options option[value='all']").prop('selected', true);
            $("#sensor_events").hide()

            if($("#idname option:selected").text() == "All Sensors" && $("#idname").val() > 5) {
                $("#idevent_options option[value='sensor']").remove();
            }
            else if($("#idevent_options option[value='sensor']").length == 0) {
                $('#idevent_options').append($('<option></option>').attr('value', "sensor").text(locale.event_filterssensor_events));
            }
        },

        sensorTypeChanged: function() {
            var selected_type = $('#idsensor_type').val();
            var sensor_type;

            $("#idevent_options option[value='all']").prop('selected', true);
            $("#sensor_events").hide()

            $('#idname').html("");
            $('#idname').append($('<option></option>').attr('value', 255).text("All Sensors"));
            
            if(selected_type == "Any") {
                $("#idevent_options option[value='sensor']").remove();
                for(i = 0; i < this.all_sensors.length; i++) {
                    $('#idname').append($('<option></option>').attr('value', this.all_sensors[i].type).text(this.all_sensors[i].name));
                }

                return;
            }
            else if(selected_type == "Temperature") {
                sensor_type = 1;
            }
            else if(selected_type == "Fan") {
                sensor_type = 4;
            }
            else if(selected_type == "Voltage") {
                sensor_type = 2;
            }
            else if(selected_type == "Discrete") {
                if($("#idname option:selected").text() == "All Sensors") {
                    $("#idevent_options option[value='sensor']").remove();
                }
                else if($("#idevent_options option[value='sensor']").length == 0) {
                    $('#idevent_options').append($('<option></option>').attr('value', "sensor").text(locale.event_filterssensor_events));
                }

                for(i = 0; i < this.all_sensors.length; i++) {
                    if(this.all_sensors[i].type > 4) {
                        $('#idname').append($('<option></option>').attr('value', this.all_sensors[i].type).text(this.all_sensors[i].name));
                    }
                }

                return;
            }

            if($("#idevent_options option[value='sensor']").length == 0) {
                $('#idevent_options').append($('<option></option>').attr('value', "sensor").text(locale.event_filterssensor_events));
            }

            for(i = 0; i < this.all_sensors.length; i++) {
                if(this.all_sensors[i].type == sensor_type) {
                    $('#idname').append($('<option></option>').attr('value', this.all_sensors[i].type).text(this.all_sensors[i].name));
                }
            }
        },

        changeGenId1: function() {
            var genval1 = 0x0;
            var slave_sw_id = parseInt($("#idslave_addr_software_id").val());
            var gen_type = $("input[name='generator_type']").filter(':checked').val();
            if(isNaN(slave_sw_id)) { slave_sw_id = 0; }

            if(gen_type == "software") {
                genval1 = 0x1;
            }

            if(slave_sw_id >= 0 && slave_sw_id <= 127) {
                genval1 = ((slave_sw_id << 1) & 0xFE) | genval1;
            }

            $("#idgenerator_id_1").val("0x" + genval1.toString(16).toUpperCase());
        },

        changeGenId2: function() {
            var genval2;
            var gen_type = $("input[name='generator_type']").filter(':checked').val();
            var ch_num = parseInt($("#idchannel_number").val());
            var ipmb_lun = parseInt($("#idipmb_lun").val());
            if(isNaN(ch_num)) { ch_num = 0; }
            if(isNaN(ipmb_lun)) { ipmb_lun = 0; }

            genval2 = (ch_num << 4) & 0xF0;

            if(gen_type == "slave") {
                genval2 = ipmb_lun | genval2;
            }

            $("#idgenerator_id_2").val("0x" + genval2.toString(16).toUpperCase());
        },

        displaySensorEvents: function() {
            if($("#idevent_options").val() != "sensor") {
                $("#sensor_events").hide()
                return;
            }

            var table_contents = "";
            $("#sensor_events").show()

            $("#sensor_events_table").html("");

            if(parseInt($("#idname").val()) < 5 || $("#idname option:selected").text() == "All Sensors") {
                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslnc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnc\" value=\"low\" id=\"0\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnc\" value=\"high\" id=\"1\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lc\" value=\"low\" id=\"2\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lc\" value=\"high\" id=\"3\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslnr + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnr\" value=\"low\" id=\"4\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnr\" value=\"high\" id=\"5\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersunc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unc\" value=\"low\" id=\"6\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unc\" value=\"high\" id=\"7\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersuc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"uc\" value=\"low\" id=\"8\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"uc\" value=\"high\" id=\"9\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersunr + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unr\" value=\"low\" id=\"10\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unr\" value=\"high\" id=\"11\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                $("#sensor_events_table").html(table_contents);
            }
            else {
                var strings = this.sensor_specific_event_strings[parseInt($("#idname").val())];

                for(i = 0; i < strings.length; i++) {
                    table_contents += "<tr>";
                    table_contents += "<td>" + strings[i] + "</td>";
                    table_contents += "<td><input type=\"checkbox\" name=\"discrete\" id=\"" + i + "\"></td>";
                    table_contents += "</tr>";
                }

                $("#sensor_events_table").html(table_contents);
            }

            $("#sensor_events_table").find('input').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                increaseArea: '20%'
            });
        },

        checkedEnableFilter: function(ev) {
            this.model.set('enable_filter', 1);
        },
        uncheckedEnableFilter: function(ev) {
            this.model.set('enable_filter', 0);
        },
        checkedRawData: function(ev) {
            this.model.set('raw_data', 1);
            $("#idgenerator_id_1,#idgenerator_id_2").removeAttr("disabled");
            $("#idgenerator_type,#idslave_addr_software_id,#idchannel_number,#idipmb_lun").attr("disabled", "disabled");
        },
        uncheckedRawData: function(ev) {
            this.model.set('raw_data', 0);
            $("#idgenerator_type,#idslave_addr_software_id,#idchannel_number,#idipmb_lun").removeAttr("disabled");
            $("#idgenerator_id_1,#idgenerator_id_2").attr("disabled", "disabled");
        },
        checkedGeneratorType: function(ev) {
            var gen_type = $(ev.target).filter(':checked').val();
            this.model.set('generator_type', gen_type);

            var genval1, genval2;
            var slave_sw_id = parseInt($("#idslave_addr_software_id").val());
            var ch_num = parseInt($("#idchannel_number").val());
            var ipmb_lun = parseInt($("#idipmb_lun").val());

            if(isNaN(slave_sw_id)) { slave_sw_id = 0; }
            if(isNaN(ch_num)) { ch_num = 0; }
            if(isNaN(ipmb_lun)) { ipmb_lun = 0; }

            if(gen_type == "slave") {
                genval1 = 0x0;
                $("#idipmb_lun").removeAttr("disabled");
            }
            else if(gen_type == "software") {
                genval1 = 0x1;
                $("#idipmb_lun").attr("disabled", "disabled");
            }

            if(slave_sw_id >= 0 && slave_sw_id <= 127) {
                genval1 = ((slave_sw_id << 1) & 0xFE) | genval1;
            }

            $("#idgenerator_id_1").val("0x" + genval1.toString(16).toUpperCase());

            genval2 = (ch_num << 4) & 0xF0;

            if(gen_type == "slave") {
                genval2 = ipmb_lun | genval2;
            }

            $("#idgenerator_id_2").val("0x" + genval2.toString(16).toUpperCase());
        },
        uncheckedGeneratorType: function() {},
        addSensors: function(model, collection) {
            var name = model.get('name');
            var type = model.get('type_number');

            if(this.sensor_num == 0xFF || this.sensor_num == type || (type > 4 && $('#idsensor_type').val() == "Discrete")) {
                $('#idname').append($('<option></option>').attr('value', type).text(name));
            }

            this.all_sensors.push({"name": name, "type": type});

            if ($("#idname").has("option:contains(" + this.sensor_val + ")").length) {
                $("#idname option:contains(" + this.sensor_val + ")").prop('selected', true);
            }
        },
        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
                } else {
                    return true;
                }
            });
            $(".tooltip").hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        updateEnableFilter: function(model, value) {
            if (value) {
                $('#idenable_filter').iCheck('check');
            } else {
                $('#idenable_filter').iCheck('uncheck');
            }
        },
        updateTriggerEventSeverity: function(model, value) {
            $('#idtrigger_event_severity').val(value);
        },
        updatePowerAction: function(model, value) {
            $('#idpower_action').val(value);
        },
        updatePolicyGroup: function(model, value) {
            $('#idpolicy_group').val(value);
        },
        updateRawData: function(model, value) {
            if (value) {
                $('#idraw_data').iCheck('check');
            } else {
                $('#idraw_data').iCheck('uncheck');
            }
        },
        updateGeneratorId1: function(model, value) {
            $('#idgenerator_id_1').val(value);
        },
        updateGeneratorId2: function(model, value) {
            $('#idgenerator_id_2').val(value);
        },
        updateGeneratorType: function(model, value) {
            $('input[name="generator_type"]').iCheck('uncheck');
            $('input[name="generator_type"][value="' + value + '"]').iCheck('check');
        },
        updateSlaveAddrSoftwareId: function(model, value) {
            $('#idslave_addr_software_id').val(value);
        },
        updateChannelNumber: function(model, value) {
            $('#idchannel_number').val(value);
        },
        updateIpmbLun: function(model, value) {
            $('#idipmb_lun').val(value);
        },
        updateSensorType: function(model, value) {
            $('#idsensor_type').val(value);

            if(value != "Any" && $("#idevent_options option[value='sensor']").length == 0) {
                $('#idevent_options').append($('<option></option>').attr('value', "sensor").text(locale.event_filterssensor_events));
            }
        },
        updateName: function(model, value) {
            this.sensor_val = value;
            //$('#idname').val(value);
        },
        updateSensorNum: function(model, value) {
            this.sensor_num = value;
            //$('#idname').val(value);
        },
        updateEventOptions: function(model, value) {
            $('#idevent_options').val(value);
        },
        updateEventTrigger: function(model, value) {
            $('#idevent_trigger').val(value);
        },
        updateEventData1OffsetMask: function(model, value) {
            if(value == 0xFFFF) {
                return;
            }

            var table_contents = "";
            $("#sensor_events").show()
            $("#sensor_events_table").html("");

            if($("#idevent_options option[value='sensor']").length == 0) {
                $('#idevent_options').append($('<option></option>').attr('value', "sensor").text(locale.event_filterssensor_events));
            }

            if($('#idsensor_type').val() == "Discrete") {
                var strings = this.sensor_specific_event_strings[this.sensor_num];

                for(i = 0; i < strings.length; i++) {
                    table_contents += "<tr>";
                    table_contents += "<td>" + strings[i] + "</td>";
                    table_contents += "<td><input type=\"checkbox\" name=\"discrete\" id=\"" + i + "\"></td>";
                    table_contents += "</tr>";
                }

                $("#sensor_events_table").html(table_contents);
            }
            else {
                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslnc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnc\" value=\"low\" id=\"0\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnc\" value=\"high\" id=\"1\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lc\" value=\"low\" id=\"2\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lc\" value=\"high\" id=\"3\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filterslnr + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnr\" value=\"low\" id=\"4\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"lnr\" value=\"high\" id=\"5\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersunc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unc\" value=\"low\" id=\"6\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unc\" value=\"high\" id=\"7\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersuc + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"uc\" value=\"low\" id=\"8\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"uc\" value=\"high\" id=\"9\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                table_contents += "<tr>";
                table_contents += "<td>" + locale.event_filtersunr + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unr\" value=\"low\" id=\"10\">" + locale.event_filtersgoing_low + "</td>";
                table_contents += "<td><input type=\"checkbox\" name=\"unr\" value=\"high\" id=\"11\">" + locale.event_filtersgoing_high + "</td>";
                table_contents += "</tr>";

                $("#sensor_events_table").html(table_contents);
            }

            $("#sensor_events_table").find('input').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                increaseArea: '20%'
            });

            for(i = 0; i < $('#sensor_events_table input').length; i++) {
                if(value & Math.pow(2, i)) {
                    $("#sensor_events_table input[id=" + i + "]").iCheck("check");
                }
            }
        },
        updateEventData1AndMask: function(model, value) {
            $('#idevent_data_1_and_mask').val(value);
        },
        updateEventData1Compare1: function(model, value) {
            $('#idevent_data_1_compare_1').val(value);
        },
        updateEventData1Compare2: function(model, value) {
            $('#idevent_data_1_compare_2').val(value);
        },
        updateEventData2AndMask: function(model, value) {
            $('#idevent_data_2_and_mask').val(value);
        },
        updateEventData2Compare1: function(model, value) {
            $('#idevent_data_2_compare_1').val(value);
        },
        updateEventData2Compare2: function(model, value) {
            $('#idevent_data_2_compare_2').val(value);
        },
        updateEventData3AndMask: function(model, value) {
            $('#idevent_data_3_and_mask').val(value);
        },
        updateEventData3Compare1: function(model, value) {
            $('#idevent_data_3_compare_1').val(value);
        },
        updateEventData3Compare2: function(model, value) {
            $('#idevent_data_3_compare_2').val(value);
        },
        deleteConfirm: function() {
            if (!confirm("Are you sure to delete?")) return;
            this.delete.call(this);
        },
        delete: function() {
            this.model.destroy({
                success: function() {
                    var path="settings/pef/event_filters";
                    app.router.navigate(path, {
                                            trigger: true
                                       });
            
                	/*//settings/pef/event_filters
                    var back_url = location.hash.split('/');
                    back_url.pop();
                    app.router.navigate(back_url.join('/'), {
                        trigger: true
                    });*/
                }
            });
        },

        validData: function(model) {
            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }
        },

        errorFields: [],

        invalidData: function(model, errors) {
            console.log('invalid', errors);
            var elMap = {
                    "policy_group": "idpolicy_group",
                    "sensor_type": "idsensor_type"
                },
                jel;

            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }

            for (var e in errors) {
                jel = this.$el.find("#" + elMap[e]);
                jel.tooltip({
                    animation: false,
                    title: errors[e],
                    trigger: 'manual',
                    placement: 'top'
                });
                jel.tooltip('show');
                this.errorFields.push(jel);


            }
            $("#save-icon").removeClass().addClass("fa fa-save");
        },

        save: function() {
            var evtData1OffsetMask = 0;
            if($("#idevent_options").val() != "sensor") {
                evtData1OffsetMask = 0xFFFF;
            }
            else {
                $('#sensor_events_table input:checked').each(function() {
                    evtData1OffsetMask += Math.pow(2, parseInt($(this).attr('id')));
                });
            }

            $("#save-icon").removeClass().addClass("ion-loading-c");


            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
            context.model.save({
                'trigger_event_severity': $('#idtrigger_event_severity').val(),
                'power_action': $('#idpower_action').val(),
                'policy_group': parseInt($('#idpolicy_group').val()),
                'generator_id_1': parseInt($('#idgenerator_id_1').val()),
                'generator_id_2': parseInt($('#idgenerator_id_2').val()),
                'generator_type': $('#idgenerator_type').val(),
                'slave_addr_software_id': $('#idslave_addr_software_id').val(),
                'channel_number': $('#idchannel_number').val(),
                'ipmb_lun': $('#idipmb_lun').val(),
                'sensor_type': $("#idsensor_type").val(), //parseInt($("#idname option:selected").val()),
                'name': $("#idname option:selected").text(),
                'event_options': $('#idevent_options').val(),
                'event_trigger': parseInt($('#idevent_trigger').val()),
                'event_data_1_offset_mask': evtData1OffsetMask,
                'event_data_1_and_mask': parseInt($('#idevent_data_1_and_mask').val()),
                'event_data_1_compare_1': parseInt($('#idevent_data_1_compare_1').val()),
                'event_data_1_compare_2': parseInt($('#idevent_data_1_compare_2').val()),
                'event_data_2_and_mask': parseInt($('#idevent_data_2_and_mask').val()),
                'event_data_2_compare_1': parseInt($('#idevent_data_2_compare_1').val()),
                'event_data_2_compare_2': parseInt($('#idevent_data_2_compare_2').val()),
                'event_data_3_and_mask': parseInt($('#idevent_data_3_and_mask').val()),
                'event_data_3_compare_1': parseInt($('#idevent_data_3_compare_1').val()),
                'event_data_3_compare_2': parseInt($('#idevent_data_3_compare_2').val())
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
                    var back_url = location.hash.split('/');
                    back_url.pop();
                    back_url.pop();
                    app.router.navigate(back_url.join('/'), {
                        trigger: true
                    });
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                }
            });


        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
