<?php

/**
 * Implements hook_from_FORM_ID_alter().
 */
function multivalue_extras_form_field_ui_field_edit_form_alter(&$form, &$form_state) {
  $instance = $form['#instance'];
  $field = $form['#field'];
  $widget = $instance['widget'];
  $settings = isset($widget['settings']['multivalue_extras'])
    ? $widget['settings']['multivalue_extras']
    : multivalue_extras_settings_defaults();

  // Is this widget using a default behaviour for multiple values?
  $field_default_behavior =  field_behaviors_widget('multiple values', $form['#instance']) == FIELD_BEHAVIOR_DEFAULT;

  // Get excluded field widgets.
  $excluded_widgets = multivalue_extras_excluded_widgets();

  // Get additional widgets having behavior different than default.
  $additional_widgets = multivalue_extras_additional_widgets();

  if (($field_default_behavior || in_array($widget['type'], $additional_widgets) && !in_array($widget['type'], $excluded_widgets))) {
    if (!empty($form_state['values']['field']['cardinality'])) {
      $cardinality = $form_state['values']['field']['cardinality'];
    }
    elseif (!empty($field['cardinality'])) {
      $cardinality = $field['cardinality'];
    }
    else {
      $cardinality = 1;
    }

    $multi_value = $cardinality == FIELD_CARDINALITY_UNLIMITED || $cardinality > 1;

    $form['instance']['widget']['settings']['multivalue_extras'] = array(
      '#prefix' => '<div id="multivalue-extras-wrapper">',
      '#suffix' => '</div>',
    );

    // Only alter the form for multi value fields.
    if ($multi_value) {
      $form['instance']['widget']['settings']['multivalue_extras'] += array(
        '#type' => 'fieldset',
        '#title' => t('Multi value settings'),
      );

      // Number of required values.
      $max_items = $cardinality;
      if ($cardinality == FIELD_CARDINALITY_UNLIMITED) {
        $max_items = 10;
      }
      $options = drupal_map_assoc(range(1, $max_items));
      if ($cardinality != FIELD_CARDINALITY_UNLIMITED) {
        $options[$cardinality] = t('@count (All)', array('@count' => $cardinality));
      }
      $form['instance']['widget']['settings']['multivalue_extras']['required_values'] = array(
        '#type' => 'select',
        '#title' => t('Number of required values'),
        '#default_value' => $settings['required_values'],
        '#description' => t('This is the minimum number of values this field is required to have.'),
        '#options' => $options,
        '#states' => array(
          'visible' => array(
            ':input[name="instance[required]"]' => array('checked' => TRUE),
          ),
        ),
      );

      // Number of values to show initially.
      if (!isset($widget['settings']['multivalue_extras']['visible_widgets']) && $cardinality != FIELD_CARDINALITY_UNLIMITED) {
        // Use default behaviour of multi value fields - show all widgets.
        $settings['visible_widgets'] = $cardinality;
      }
      $form['instance']['widget']['settings']['multivalue_extras']['visible_widgets'] = array(
        '#type' => 'select',
        '#title' => t('Number of visible field widgets'),
        '#default_value' => $settings['visible_widgets'],
        '#description' => t('This is the number of field widgets to display event if the field is empty. Button "Add another" will be displayed to add more items.'),
        '#options' => $options,
      );

      // Label for "Add another" button.
      $form['instance']['widget']['settings']['multivalue_extras']['add_another_label'] = array(
        '#type' => 'textfield',
        '#title' => t('Label of button to add additional items'),
        '#description' => t('Override label of the button to add another item (if displayed). Leave empty to use default (Add another).', array('@field_label' => strtolower($instance['label']))),
        '#default_value' => $settings['add_another_label'],
      );

      $form['instance']['widget']['settings']['multivalue_extras']['weight_type'] = array(
        '#type' => 'radios',
        '#title' => t('Type for weight ordering'),
        '#default_value' => $settings['weight_type'],
        '#options' => array(
          'draggable' => t('Draggable table'),
          'defaultable' => t('Defaultable value'),
          'none' => t('None'),
        ),
      );

      // Remove "Hide blank items" checkbox delivered by field_collection module.
      if (isset($form['field']['settings']['hide_blank_items'])) {
        $form['field']['settings']['hide_blank_items']['#access'] = FALSE;
      }

      $form['#validate'][] = 'multivalue_extras_validate_settings';
    }
    else {
      $form['instance']['widget']['settings']['multivalue_extras']['#type'] = 'container';
    }

    $form['field']['cardinality']['#ajax'] = array(
      'callback' => 'multivalue_extras_cardinality_ajax_callback',
      'wrapper' => 'multivalue-extras-wrapper',
    );
  }
}

/**
 * Ajax callback for mulivalue extras fieldset.
 */
function multivalue_extras_cardinality_ajax_callback($form, &$form_state) {
  return $form['instance']['widget']['settings']['multivalue_extras'];
}

/**
 * Returns an array of default values to use for the admin form.
 */
function multivalue_extras_settings_defaults() {
  return array(
    'required_values' => 1,
    'visible_widgets' => 1,
    'add_another_label' => '',
    'weight_type' => 'draggable',
  );
}

/**
 * Validate handler for the field settings.
 */
function multivalue_extras_validate_settings($form, &$form_state) {
  $multivalue_settings = $form_state['values']['instance']['widget']['settings']['multivalue_extras'];

  $field = $form_state['values']['field'];

  $is_multi_value = $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED || $field['cardinality'] > 1;
  if ($is_multi_value && isset($form_state['values']['field']['settings']['hide_blank_items']) && $multivalue_settings['visible_widgets'] != $field['cardinality']) {
    $form_state['values']['field']['settings']['hide_blank_items'] = 0;
  }
}

