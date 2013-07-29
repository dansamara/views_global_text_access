<?php
/**
 * @file
 * Views handler: Global:Text with access check.
 */
   
/**
  * Provide markup text for the area with access check form.
  */
class views_global_text_access_handler_area_text extends views_handler_area_text {

  function option_definition() {
    $options = parent::option_definition();
    $options['access_check_type'] = array('default' => 'perm');
    $options['access_check_perm'] = array('default' => 'access content');
    $options['access_check_role'] = array('default' => array());
    return $options;
  }
                        
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    // return;

    $form['label']['#weight'] = -10;
    
    $form['access'] = array(
      '#title' => 'Access restrictions',
      '#type' => 'fieldset', 
      '#weight' => -5, 
      '#collapsible' => true,
      '#collapsed' => false,
    );
    $form['access_check_type'] =  array(
      '#title' => 'Access restrictions by…',
      '#type' => 'radios',
      '#options' => array('role' => 'Role', 'perm' => 'Permissions'),
      '#default_value' => $this->options['access_check_type'],
      '#fieldset' => 'access',
    );

    $form['access_check_role'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Role'),
      '#default_value' => $this->options['access_check_role'],
      '#options' => array_map('check_plain', views_ui_get_roles()),
      '#description' => t('Only the checked roles will be able to access view this field.'),
      '#fieldset' => 'access',
      '#states' => array(
        'visible' => array(
          ':input[name="options[access_check_type]"]' => array('value' => 'role'),
        ),
      ),
    );

    $perms = array();
    $module_info = system_get_info('module');

    // Get list of permissions
    foreach (module_implements('permission') as $module) {
      $permissions = module_invoke($module, 'permission');
      foreach ($permissions as $name => $perm) {
        $perms[$module_info[$module]['name']][$name] = strip_tags($perm['title']);
      }
    }

    ksort($perms);

    $form['access_check_perm'] = array(
      '#type' => 'select',
      '#options' => $perms,
      '#title' => t('Permission'),
      '#default_value' => $this->options['access_check_perm'],
      '#description' => t('Only users with the selected permission flag will be able to access view this field.'),
      '#fieldset' => 'access',
      '#states' => array(
        'visible' => array(
          ':input[name="options[access_check_type]"]' => array('value' => 'perm'),
        ),
      ),
    );
  }

  function options_validate(&$form, &$form_state) {
    if ($form_state['values']['options']['access_check_type'] == 'role'
        && !array_filter($form_state['values']['options']['access_check_role'])) {
      form_error($form['access_check_role'], t('You must select at least one role if type is "by role"'));
    }
  }

  function options_submit(&$form, &$form_state) {
    // I hate checkboxes too.
    $form_state['values']['options']['access_check_role'] = array_keys(array_filter($form_state['values']['options']['access_check_role']));
    parent::options_submit($form, $form_state);
  }

  /**
    * Overrides views_handler_area_text::render().
    */
  function render($empty = FALSE) {
    dsm($this->options);
    switch ($this->options['access_check_type']) {
      case 'role':
        global $user;
        $roles = array_keys($user->roles);
        $roles[] = $user->uid ? DRUPAL_AUTHENTICATED_RID : DRUPAL_ANONYMOUS_RID;
        $access = array_intersect(array_filter($this->options['access_check_role']), $roles);
        break;
      case 'perm':
        $access = user_access($this->options['access_check_perm']);
        break;
      default:
        $access = false;
    }
    if ($access && (!$empty || !empty($this->options['empty']))) {
      $format = isset($this->options['format']) ? $this->options['format'] : filter_default_format();
      return $this->render_textarea($this->options['content'], $format);
    }
    return '';
  }
}
