<?php

/**
 * @file
 * Functions for assembling report.
 */

function islandora_repo_reporter_report() {
  module_load_include('inc', 'islandora_repo_reporter', 'includes/utilities');
  $items = array();

  $object_count = islandora_repo_reporter_get_repo_object_count();
  $items['islandora_repo_reporter_object_count'] = array('title' => t('Number of objects in repository'), 'value' => $object_count);

  $oldest = islandora_repo_reporter_get_object_created_on('ASC');
  $items['islandora_repo_reporter_oldest_object'] = array('title' => t('Oldest object'), 'value' => $oldest[0] . ' (created ' . $oldest[1] . ')');

  $newest = islandora_repo_reporter_get_object_created_on('DESC');
  $items['islandora_repo_reporter_newest_object'] = array('title' => t('Newest object'), 'value' => $newest[0] . ' (created ' . $newest[1] . ')');

  $collection_count = islandora_repo_reporter_get_collection_object_count();
  $items['islandora_repo_reporter_collection_count'] = array('title' => t('Number of collection objects'), 'value' => $collection_count);

  $policy_count = islandora_repo_reporter_count_policy_datastreams();
  $items['islandora_repo_reporter_policy_count'] = array('title' => t('Number of objects with POLICY datastreams'), 'value' => $policy_count);

  $num_pids_in_solr = islandora_repo_reporter_query_solr();
  $items['islandora_repo_reporter_pids_in_solr'] = array('title' => t('Number of objects indexed in Solr'), 'value' => $num_pids_in_solr);

  $cmodel_pids = islandora_repo_reporter_get_content_models();
  $cmodel_list = array();
  $cmodel_list['type'] = 'ul';
  $cmodel_list['items'] = array();
  foreach ($cmodel_pids as $cmodel_pid) {
    $cmodel_list['items'][] = $cmodel_pid;
  }
  $cmodel_list_output = theme('item_list', $cmodel_list);
  $items['islandora_repo_reporter_installed_cmodels'] = array('title' => t('Content models installed'), 'value' => $cmodel_list_output);

  $module_names = islandora_repo_reporter_get_islandora_modules();
  $module_list = array();
  $module_list['type'] = 'ul';
  $module_list['items'] = array();
  foreach ($module_names as $module_name) {
    $module_list['items'][] = $module_name;
  }
  $module_list_output = theme('item_list', $module_list);
  $items['islandora_repo_reporter_islandora_modules'] = array('title' => t('Islandora modules enabled'), 'value' => $module_list_output);

  $element_list = array();
  if (variable_get('islandora_repo_reporter_is_running', FALSE)) {
    // We are running so we can't report.
    $element_list_output = t('The MODS investigation is still in process, check back later');
  }
  else {
    // We aren't running, do we have elements?
    $elements = islandora_repo_reporter_get_elements();
    // Do we have any data?
    if (count($elements) > 0) {
      $element_list['type'] = 'ul';
      $element_list['items'] = array_keys($elements);
      $element_list_output = theme('item_list', $element_list);
    }
    else {
      $element_list_output = l('No data, please run the cron job by clicking this link', 'admin/reports/islandora_repo_report/run');
    }
  }
  $items['islandora_repo_reporter_mods_elements'] = array('title' => t('MODS elements used'), 'value' => $element_list_output);

  return theme('status_report', array('requirements' => $items));
}
