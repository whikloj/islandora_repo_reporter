<?php

/**
 * @file
 * Utilities.
 */

/**
 * Queries the RI to get number of objects in the repo.
 *
 * @return int
 *   Number of objects returned by the query.
 */
function islandora_repo_reporter_get_repo_object_count() {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection();

  $ri_query = <<<EOQ
  SELECT ?object
  WHERE {
    ?object <fedora-model:hasModel> <info:fedora/fedora-system:FedoraObject-3.0> .
  }
EOQ;

  $ri = $connection->repository->ri;
  return $ri->countQuery($ri_query, 'sparql');
}

/**
 * Queries the RI to get number of collection objects in the repo.
 *
 * @return int
 *   Number of objects returned by the query.
 */
function islandora_repo_reporter_get_collection_object_count() {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection();

  $ri_query = <<<EOQ
  SELECT ?object
  WHERE {
    ?object <fedora-model:hasModel> <info:fedora/islandora:collectionCModel> .
  }
EOQ;

  $ri = $connection->repository->ri;
  return $ri->countQuery($ri_query, 'sparql');
}

/**
 * Queries the RI to get the most recent or earlies object in repo.
 *
 * @param string $order
 *   The order to sort in, either 'ASC' or 'DESC'.
 *
 * @return array
 *   An array containing the PID of the objec and the date.
 */
function islandora_repo_reporter_get_object_created_on($order) {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection();

  $ri_query = <<<EOQ
  SELECT ?object ?created
  FROM <#ri>
  WHERE {
    ?object <fedora-model:createdDate> ?created .
  }
  ORDER BY $order(?created)
EOQ;

  $result = $connection->repository->ri->sparqlQuery($ri_query, 1);
  return array($result[0]['object']['value'], $result[0]['created']['value']);
}

/**
 * Queries the RI to get the number of objects that have a POLICY datastream.
 *
 * @return int
 *   Number of objects returned by the query.
 */
function islandora_repo_reporter_count_policy_datastreams() {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection();

  $ri_query = <<<EOQ
  SELECT ?pid
  FROM <#ri>
  WHERE {
    ?dss <fedora-view:disseminationType> <info:fedora/*/POLICY> .
    ?pid <info:fedora/fedora-system:def/view#disseminates> 
  ?dss
  }
EOQ;

  $ri = $connection->repository->ri;
  return $ri->countQuery($ri_query, 'sparql');
}

/**
 * Queries the RI to get the content models that are installed in the repo.
 *
 * @return array
 *   An array containing the PIDs of installed content models.
 */
function islandora_repo_reporter_get_content_models() {
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection();

  $cmodels = array();
  $ri_query = <<<EOQ
  SELECT ?object
  WHERE {
    ?object <fedora-model:hasModel> <info:fedora/fedora-system:ContentModel-3.0> .
  }
EOQ;

  $result = $connection->repository->ri->sparqlQuery($ri_query, 'unlimited');
  foreach ($result as $cmodel) {
    $cmodels[] = $cmodel['object']['value'];
  }

  return $cmodels;
}

/**
 * Queries the Drupal {system} table to get a list of enabled Islandora modules.
 *
 * @return array
 *   The modules.
 */
function islandora_repo_reporter_get_islandora_modules() {
  $result = db_select('system')
   ->fields('system', array('name'))
   ->condition('status', 1)
   ->condition('name', db_like('islandora') . '%', 'LIKE')
   ->orderBy('name', 'ASC')
   ->execute()->fetchAll();

  $names = array();
  foreach ($result as $row) {
    $names[] = $row->name;
  }
  return $names;
}

/**
 * Queries Islandora's Solr index to get the number of indexed objects.
 *
 * @return int
 *   The number of PIDs Solr knows about.
 */
function islandora_repo_reporter_total_solr_records() {
  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = "PID:*";
  $query_processor->executeQuery(FALSE);
  $num_pids = $query_processor->islandoraSolrResult['response']['numFound'];
  return $num_pids;
}

/**
 * Recursive function to walk an XML document leafs accumulating XPaths.
 *
 * @param \DOMNode $element
 *   The current element.
 * @param array $parents
 *   The parents array.
 *
 * @throws \Exception
 *   In case of SQL error.
 */
function _islandora_repo_reporter_traverse(DOMNode $element, array $parents = array()) {
  $parents[] = $element->tagName;
  $numchildren = $element->childNodes->length;
  $element_kids = FALSE;
  if ($numchildren > 0) {
    for ($foo = 0; $foo < $numchildren; $foo += 1) {
      $child = $element->childNodes->item($foo);
      if ($child instanceof DOMElement) {
        $element_kids = TRUE;
        _islandora_repo_reporter_traverse($child, $parents);
      }
    }
  }
  if (!$element_kids) {
    _islandora_repo_reporter_report_mods_path($parents);
  }
}

/**
 * Utility to populate the reporter table with the mods xpaths.
 *
 * @param array $parents array of parents elements.
 * @throws \Exception On sql error.
 */
function _islandora_repo_reporter_report_mods_path(array $parents) {
  try {
    db_insert('islandora_repo_reporter_mods_elements')
      ->fields(array(
        'elements' => '/' . implode('/', $parents),
      ))->execute();
  } catch (PDOException $e) {
    // Unique key exception so we can swallow it.
  }
}

/**
 * Populate the check objects queue.
 *
 * @param int $limit Number of items to return this time.
 *
 * @return array Array of pids to check.
 */
function islandora_repo_reporter_populate_object_queue($limit) {
  global $user;
  // Need to switch to admin to get all objects.
  $original_user = $user;
  $old_state = drupal_save_session();
  drupal_save_session(FALSE);
  $user = user_load(1);

  $offset = (int) variable_get('islandora_repo_reporter_mods_check_offset', 0);

  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = "PID:*";
  $query_processor->solrParams['sort'] = 'PID asc';
  $query_processor->solrParams['fl'] = array('PID');
  $query_processor->solrParams['rows'] = $limit;
  $query_processor->solrLimit = $limit;
  $query_processor->solrStart = $offset;

  $query_processor->executeQuery(FALSE);
  $pids = $query_processor->islandoraSolrResult['response']['objects'];

  array_walk($pids, function(&$item) {
    $item = $item['PID'];
  });

  if (count($pids) < $limit) {
    $offset = 0;
    variable_set('islandora_repo_reporter_is_running', FALSE);
  }
  else {
    $offset = (int) $offset + (int) $limit;
  }

  variable_set('islandora_repo_reporter_mods_check_offset', $offset);

  // Switch back to user
  $user = $original_user;
  drupal_save_session($old_state);

  return $pids;
}

/**
 * Get the MODS elements from the database, if any.
 *
 * @return array
 *   Of MODS element names.
 */
function islandora_repo_reporter_get_elements() {
  $result = db_query('SELECT elements FROM {islandora_repo_reporter_mods_elements}');
  return $result->fetchAllAssoc('elements');
}

/**
 * Start the MODS investigation.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The updated Drupal form.
 */
function islandora_repo_reporter_run_mods_check($form, &$form_state) {
  return confirm_form(
    $form,
    t("Do you really want to start the MODS element check?"),
    'admin/reports/islandora_repo_report',
    t("This will slowly (via CRON) parse all the MODS records in your" .
      " repository and generate a list of distinct XPaths. No attributes nor" .
      " text content is stored. You can see the elements on the Repo Reporter" .
      " page once it is complete."),
    t("Start")
  );
}

/**
 * Implements hook_FORMID_submit().
 */
function islandora_repo_reporter_run_mods_check_submit($form, &$form_state) {
  if (!variable_get('islandora_repo_reporter_is_running', FALSE)) {
    variable_set('islandora_repo_reporter_is_running', TRUE);
  }
  $form_state['redirect'] = 'admin/reports/islandora_repo_report';
}
