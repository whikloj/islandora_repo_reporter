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
 * @return int
 *   The date.
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
function islandora_repo_reporter_query_solr() {
  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = "PID:*";
  $query_processor->executeQuery(FALSE);
  $num_pids = $query_processor->islandoraSolrResult['response']['numFound'];
  return $num_pids;
}
