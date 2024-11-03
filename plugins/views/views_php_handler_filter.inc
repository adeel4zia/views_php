<?php

/**
 * A handler to filter a view using PHP defined by the administrator.
 *
 * @ingroup views_filter_handlers
 */
class views_php_handler_filter extends views_handler_filter {

  protected $php_static_variable = NULL;

  /**
   * Implements views_handler#can_expose().
   */
  public function can_expose() {
    return FALSE;
  }

  /**
   * Implements views_object#admin_summary().
   */
  public function admin_summary() {
    return t('PHP');
  }

  /**
   * Implements views_object#option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['use_php_setup'] = ['default' => FALSE];
    $options['php_setup'] = ['default' => ''];
    $options['php_filter'] = ['default' => ''];
    return $options;
  }

  /**
   * Implements views_handler#options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['use_php_setup'] = [
      '#type' => 'checkbox',
      '#title' => t('Use setup code'),
      '#description' => t('If checked, you can provide PHP code to be run once right before execution of the view. This may be useful to define functions to be reused in the filter code.'),
    ];

    $form['php_setup'] = [
      '#type' => 'textarea',
      '#title' => t('Setup code'),
      '#description' => t('Code to run right before execution of the view.'),
      '#states' => [
        'visible' => [
          ':input[name="use_php_setup"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['php_filter'] = [
      '#type' => 'textarea',
      '#title' => t('Filter code'),
      '#description' => t('If the code returns TRUE, the current row is removed from the results.'),
    ];
  }

  /**
   * Implements views_handler_filter#query().
   */
  public function query() {
    // Inform Backdrop to take control of the query.
    $this->view->views_php = TRUE;
    $this->view->views_php_query_all = TRUE;
  }

  /**
   * Execute static PHP code before running the query.
   */
  public function php_pre_execute() {
    if (!empty($this->options['php_setup'])) {
      $code = $this->options['php_setup'] . ';';
      eval($code);
    }
  }

  /**
   * Execute the custom PHP filtering logic after the query.
   */
  public function php_post_execute() {
    if (!empty($this->options['php_filter'])) {
      $code = $this->options['php_filter'] . ';';

      foreach ($this->view->result as $i => $row) {
        $normalized_row = new stdClass();
        foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
          $normalized_row->$field = isset($row->{$handler->field_alias}) ? $row->{$handler->field_alias} : NULL;
        }

        $result = eval($code);
        if ($result) {
          unset($this->view->result[$i]);
        }
      }
    }
  }

}
