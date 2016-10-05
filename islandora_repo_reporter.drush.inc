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
      'drush ireporepo --user=admin --output_file=/tmp/output.txt',
    ),
    'options' => array(
      'output_file' => array(
        'description' => 'Path to the file to write the report to. Leave blank to print to STDOUT',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Runs the Islandora Repo Reporter reports.
 */
function drush_islandora_repo_reporter() {
  if (drush_get_option('output_file')) {
    // @todo: Print to a file.
    $output_file = drush_get_option('output_file');
  }
  else {
    $output_file = 'STDOUT';
  }
  module_load_include('inc', 'islandora_repo_reporter', 'includes/utilities');
  $object_count = islandora_repo_reporter_get_repo_object_count();
  drush_print(t("Total number of objects in this repo: !count", array('!count' => $object_count)));

  $collection_count = islandora_repo_reporter_get_collection_object_count();
  drush_print(t("Number of collection objects in this repo: !count", array('!count' => $collection_count)));

  $oldest = islandora_repo_reporter_get_object_created_on('ASC');
  drush_print(t("Oldest object created on !oldest", array('!oldest' => $oldest)));

  $newest = islandora_repo_reporter_get_object_created_on('DESC');
  drush_print(t("Newest object created on !newest", array('!newest' => $newest)));
}