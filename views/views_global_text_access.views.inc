<?php

/**
 * @file
 * Views hooks for views_global_text_access.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function views_global_text_access_views_data_alter(&$data) {
  $data['views']['views_global_text_access_global_text'] = array(
    'title' => t('Text with access check'),
    'help' => t('Provide markup text for the area with access check form'),
    'area' => array(
      'handler' => 'views_global_text_access_handler_area_text',
    ),
  );

  return $data;
}
