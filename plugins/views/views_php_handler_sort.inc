<?php

/**
 * A handler to sort a view using PHP defined by the administrator.
 *
 * @ingroup views_sort_handlers
 */
class views_php_handler_sort extends views_handler_sort {

  protected $php_static_variable = NULL;

  /**
   * Implements views_object#option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['use_php_setup'] = ['default' => FALSE];
    $options['php_setup'] = ['default' => ''];
    $options['php_sort'] = ['default' => ''];
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
      '#description' => t('If checked, you can provide PHP code to be run once right before execution of the view. This may be useful to define functions to be re-used in the value and/or output code.'),
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

    $form['php_sort'] = [
      '#type' => 'textarea',
      '#title' => t('Sort code'),
      '#description' => t('The comparison code must return an integer less than, equal to, or greater than zero if the first row should respectively appear before, stay where it was compared to, or appear after the second row.'),
    ];
  }

  /**
   * Implements views_handler_sort#query().
   */
  public function query() {
    // Inform Backdrop to take control over the query.
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
   * Execute the custom PHP sorting logic after the query.
   */
  public function php_post_execute() {
    if (!empty($this->options['php_sort']) && $this->view->style_plugin->build_sort()) {
      usort($this->view->result, [$this, 'php_sort']);
    }
  }

  /**
   * Helper function for usort() callback for sort support.
   */
  public function php_sort($row1, $row2) {
    $factor = strtoupper($this->options['order']) == 'ASC' ? 1 : -1;

    $code = $this->options['php_sort'] . ';';
    $result = eval($code);

    $normalized_row1 = $this->normalize_row($row1);
    $normalized_row2 = $this->normalize_row($row2);

    return $factor * (int) eval($code);
  }

  /**
   * Helper function to normalize rows for sorting.
   */
  protected function normalize_row($row) {
    $normalized_row = new stdClass();
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      $normalized_row->$field = isset($row->{$handler->field_alias}) ? $row->{$handler->field_alias} : NULL;
    }
    return $normalized_row;
  }

}
