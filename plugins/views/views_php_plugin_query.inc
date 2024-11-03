<?php

/**
 * A (fake) pager plugin that wraps around the actual query.
 *
 * @ingroup views_query_plugins
 */
class views_php_plugin_query extends views_php_plugin_wrapper {

  /**
   * Executes the query and wraps the pager with a PHP-based plugin.
   *
   * @param object $view
   *   The view object.
   */
  public function execute(&$view) {
    $pager = new views_php_plugin_pager();
    $pager->php_wrap($this->wrapped->pager);

    $this->wrapped->execute($view);

    $pager->php_unwrap();
  }

}
