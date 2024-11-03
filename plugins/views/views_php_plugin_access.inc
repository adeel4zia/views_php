<?php

/**
 * Access plugin that provides access control based on custom PHP code.
 *
 * @ingroup views_access_plugins
 */
class views_php_plugin_access extends views_plugin_access {

  /**
   * Implements views_plugin_access::summary_title().
   */
  public function summary_title() {
    return t('PHP');
  }

  /**
   * Implements views_plugin_access::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['php_access'] = ['default' => ''];
    return $options;
  }

  /**
   * Implements views_plugin::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['php_access'] = [
      '#type' => 'textarea',
      '#title' => t('Access code'),
      '#description' => t('If the code returns TRUE the requesting user is granted access to the view.'),
      '#default_value' => $this->options['php_access'],
    ];

    $form['php_access_info'] = [
      '#markup' => t('Available variables: <code>$view_name</code> (the name of the view), <code>$display_id</code> (the display ID), <code>$account</code> (the account object).'),
    ];
  }

  /**
   * Implements views_plugin::options_submit().
   */
  public function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);
    $this->options['php_access'] = $form_state['values']['php_access'];
  }

  /**
   * Implements views_plugin_access::access().
   */
  public function access($account) {
    if (!empty($this->options['php_access'])) {
      return $this->check_php_access($this->options['php_access'], $this->view->name, $this->display->id, $account);
    }
    return TRUE;
  }

  /**
   * Helper function to evaluate PHP access code.
   */
  protected function check_php_access($php_code, $view_name, $display_id, $account) {
    $access = FALSE;
    try {
      ob_start();
      $access = eval($php_code);
      ob_end_clean();
    }
    catch (Exception $e) {
      watchdog_exception('views_php', $e);
    }
    return $access === TRUE;
  }

  /**
   * Implements views_plugin_access::get_access_callback().
   */
  public function get_access_callback() {
    if (!empty($this->options['php_access'])) {
      return ['views_php_check_access', [$this->options['php_access'], $this->view->name, $this->display->id]];
    }
    return TRUE;
  }
}
