<?php

/**
 * @file
 * Drush file for the Islandora Repo Reporter module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_repo_reporter_drush_command() {
  $items = array();
  $items['islandora_repo_reporter'] = array(
    'aliases' => array('ireporepo'),
    'description' => 'Runs the Islandora Repo Reporter reports.',
    'examples' => array(
      'drush ireporepo --user=admin',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Runs the Islandora Repo Reporter reports.
 */
function drush_islandora_repo_reporter() {
  module_load_include('inc', 'islandora_repo_reporter', 'includes/utilities');

  $object_count = islandora_repo_reporter_get_repo_object_count();
  drush_print(t("Total number of objects in repository: !count", array('!count' => $object_count)));

  $oldest = islandora_repo_reporter_get_object_created_on('ASC');
  drush_print(t("Oldest object (!pid) created on !date", array('!pid' => $oldest[0], '!date' => $oldest[1])));

  $newest = islandora_repo_reporter_get_object_created_on('DESC');
  drush_print(t("Newest object (!pid) created on !date", array('!pid' => $newest[0], '!date' => $newest[1])));

  $collection_count = islandora_repo_reporter_get_collection_object_count();
  drush_print(t("Number of collection objects: !count", array('!count' => $collection_count)));

  $policy_count = islandora_repo_reporter_count_policy_datastreams();
  drush_print(t("Number of objects that have POLICY datastreams: !count", array('!count' => $policy_count)));

  $num_pids_in_solr = islandora_repo_reporter_total_solr_records();
  drush_print(t("Number of objects indexed in Solr: !num_objects_in_solr", array('!num_objects_in_solr' => $num_pids_in_solr)));

  $cmodel_pids = islandora_repo_reporter_get_content_models();
  $cmodels = PHP_EOL;
  foreach ($cmodel_pids as $cmodel) {
    $cmodels .= ' ' . $cmodel . PHP_EOL;
  }
  $cmodels = rtrim($cmodels);
  drush_print(t("Content models installed: !cmodels", array('!cmodels' => $cmodels)));

  $module_names = islandora_repo_reporter_get_islandora_modules();
  $names = PHP_EOL;
  foreach ($module_names as $name) {
    $names .= ' ' . $name . PHP_EOL;
  }
  $names = rtrim($names);
  drush_print(t("Islandora modules enabled: !modules", array('!modules' => $names)));
}
