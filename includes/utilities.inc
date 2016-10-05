<?php

/**
 * @file
 * Utilities.
 */

/**
 * Queries the resource index to get number of objects in repo.
 *
 * @return int
 *   Number of objects returned by the query.
 */
function islandora_repo_reporter_get_repo_object_count() {
  $user = user_load(1);
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection($user);

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
 * Queries the resource index to get number of collection objects in repo.
 *
 * @return int
 *   Number of objects returned by the query.
 */
function islandora_repo_reporter_get_collection_object_count() {
  $user = user_load(1);
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection($user);

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
 * Queries the resource index to get the most recent created on date in repo.
 *
 * @return int
 *   The date.
 */
function islandora_repo_reporter_get_object_created_on($order) {
  $user = user_load(1);
  drupal_static_reset('islandora_get_tuque_connection');
  $connection= islandora_get_tuque_connection($user);

  $ri_query = <<<EOQ
  SELECT ?object ?created
  FROM <#ri>
  WHERE {
    ?object <fedora-model:createdDate> ?created .
  }
  ORDER BY $order(?created)
EOQ;

  $result = $connection->repository->ri->sparqlQuery($ri_query, 1);
  return $result[0]['created']['value'];
}
