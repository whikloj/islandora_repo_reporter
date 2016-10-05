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

  $collection_count = islandora_repo_reporter_get_collection_object_count();
  $items['islandora_repo_reporter_collection_count'] = array('title' => t('Number of collection objects'), 'value' => $collection_count);

  $oldest = islandora_repo_reporter_get_object_created_on('ASC');
  $items['islandora_repo_reporter_oldest_object'] = array('title' => t('Oldest object'), 'value' => $oldest[0] . ' (created ' . $oldest[1] . ')');

  $newest = islandora_repo_reporter_get_object_created_on('DESC');
  $items['islandora_repo_reporter_newest_object'] = array('title' => t('Newest object'), 'value' => $newest[0] . ' (created ' . $newest[1] . ')');

  return theme('status_report', array('requirements' => $items));
}
