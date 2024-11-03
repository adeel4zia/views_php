<?php

/**
 * A handler to provide an area that is constructed by the administrator using PHP.
 *
 * @ingroup views_area_handlers
 */
class views_php_handler_area extends views_handler_area {

  /**
   * Defines the options available for this handler.
   *
   * @return array
   *   An associative array of options.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['php_output'] = array('default' => "<h4>Example PHP code</h4>\n<p>Time: <?php print date('H:i', time());?></p>\n");
    return $options;
  }

  /**
   * Builds the form for editing the options of this handler.
   *
   * @param array $form
   *   The form structure to be built.
   * @param array &$form_state
   *   The state of the form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form += views_php_form_element($this,
      FALSE,
      array('php_output', t('Output code'), t('Code to construct the output of this area.'), TRUE),
      array('$view', '$handler', '$results')
    );
  }

  /**
   * Renders the output for this handler.
   *
   * @param bool $empty
   *   Whether the area is being rendered for empty results.
   *
   * @return string
   *   The rendered output.
   */
  public function render($empty = FALSE) {
    // Execute output PHP code.
    if ((!$empty || !empty($this->options['empty'])) && !empty($this->options['php_output'])) {
      $code = ' ?>' . $this->options['php_output'];
      $function = function ($view, $handler, $results) use ($code) {
        eval($code);
      };
      ob_start();
      $function($this->view, $this, $this->view->result);
      return ob_get_clean();
    }
    return '';
  }

}
