<?php


/**
 * Class that allows searching the site with Apache Solr through a view.
 */
class apachesolr_views_handler_argument extends views_handler_argument {
  
  /**
   * Add argument to query.
   */
  public function query() {
    if (!empty($this->options['break_phrase'])) {
      $this->value = explode(',', $this->argument);
    }
    else {
      $this->value = array($this->argument);
    }
    foreach ($this->value as $facet_value) {
      $this->query->add_filter($this->real_field, apachesolr_views_query::escape_term($facet_value), $this->options['not']);
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['break_phrase'] = array('default' => FALSE);
    $options['facet_block'] = array('default' => TRUE);
    $options['not'] = array('default' => FALSE);

    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // allow for , delimited values
    $form['break_phrase'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow multiple terms per argument.'),
      '#description' => t('If selected, users can enter multiple arguments in the form of 1,2,3.'),
      '#default_value' => !empty($this->options['break_phrase']),
    );

    // Make it possible to exclude an argument from being populated with values
    // from facet blocks, e.g. if this argument is passed as part of the menu
    // link or by other means and you still want to be able to use facet blocks
    // of the same field without losing the menu link.
    // @see apachesolr_views_query->part_of_facet_block().
    $form['facet_block'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow this argument to be populated from facet blocks.'),
      '#description' => t('If unchecked, this argument will not be populated with values from Apachesolr facet block but only with values that come directly from the URL.'),
      '#default_value' => !empty($this->options['facet_block']),
    );

    $form['not'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude the argument'),
      '#description' => t('If selected, the numbers entered in the argument will be excluded rather than limiting the view.'),
      '#default_value' => !empty($this->options['not']),
    );
  }
  
  /**
   * Provide a list of default behaviors for this argument if the argument
   * is not present.
   *
   * Provide fewer methods that the standard. Remove summary views
   */
  function default_actions($which = NULL) {
    $defaults = array(
      'ignore' => array(
        'title' => t('Display all values'),
        'method' => 'default_ignore',
        'breadcrumb' => TRUE, // generate a breadcrumb to here
      ),
      'not found' => array(
        'title' => t('Hide view / Page not found (404)'),
        'method' => 'default_not_found',
        'hard fail' => TRUE, // This is a hard fail condition
      ),
      'empty' => array(
        'title' => t('Display empty text'),
        'method' => 'default_empty',
        'breadcrumb' => TRUE, // generate a breadcrumb to here
      ),
      'default' => array(
        'title' => t('Provide default argument'),
        'method' => 'default_default',
        'form method' => 'default_argument_form',
        'has default argument' => TRUE,
        'default only' => TRUE, // this can only be used for missing argument, not validation failure
      ),
    );

    if ($which) {
      if (!empty($defaults[$which])) {
        return $defaults[$which];
      }
    }
    else {
      return $defaults;
    }
  }
  
}
