<?php

/**
 * @file
 * Field type declarations and hooks for Rooms Availability Constraints module.
 */

/**
 * Implements hook_field_info().
 */
function rooms_availability_constraints_field_info() {
  return array(
    'rooms_availability_constraints_range' => array(
      'label' => t('Rooms Availability Constraints'),
      'description' => t('Bookable unit availability options.'),
      'settings' => array(),
      'default_widget' => 'rooms_availability_constraints_range',
      'default_formatter' => 'rooms_availability_constraints_range',
      'property_type' => 'rooms_availability_constraints_range',
      'property_callbacks' => array('rooms_availability_constraints_range_property_info_callback'),
    ),
  );
}

/**
 * Property callback for the Entity Metadata framework.
 */
function rooms_availability_constraints_range_property_info_callback(&$info, $entity_type, $field, $instance, $field_type) {
  // Apply the default.
  entity_metadata_field_default_property_callback($info, $entity_type, $field, $instance, $field_type);

  // Finally add in instance specific property info.
  $name = $field['field_name'];
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];
  $property['type'] = ($field['cardinality'] != 1) ? 'list<rooms_availability_constraints_range>' : 'rooms_availability_constraints_range';
  $property['property info'] = rooms_availability_constraints_range_data_property_info('Rooms availability constraints');
  $property['getter callback'] = 'entity_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_metadata_field_verbatim_set';
}

/**
 * Defines info for the properties of the Availability Constraints Range
 * data structure.
 */
function rooms_availability_constraints_range_data_property_info($name = NULL) {
  // Build an array of properties for an Availability Constraints Range.
  $properties = array(
    'start_date' => array(
      'label' => 'Start date',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'end_date' => array(
      'label' => 'End date',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'always' => array(
      'label' => 'Always enabled',
      'type' => 'boolean',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'constraint_type' => array(
      'label' => 'Constraint type',
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'start_day' => array(
      'label' => 'Starting weekday',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'minumum_stay' => array(
      'label' => 'Minimum stay',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'maximum_stay' => array(
      'label' => 'Maximum stay',
      'type' => 'integer',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    ),
  );

  // Add the default values for each property.
  foreach ($properties as &$value) {
    $value += array(
      'description' => !empty($name) ? t('!label of field %name', array('!label' => $value['label'], '%name' => $name)) : '',
    );
  }

  return $properties;
}

/**
 * Implements hook_field_is_empty().
 */
function rooms_availability_constraints_field_is_empty($item, $field) {
  if ($item['group_conditions']['operation'] == 0) {
    return TRUE;
  }
  elseif ($item['group_conditions']['operation'] == 1) {
    if (!($item['group_conditions']['minimum_stay']) && !($item['group_conditions']['maximum_stay'])) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Implements hook_field_widget_info().
 */
function rooms_availability_constraints_field_widget_info() {
  return array(
    'rooms_availability_constraints_range' => array(
      'label' => t('Availability range'),
      'description' => t('Availability range widget'),
      'field types' => array('rooms_availability_constraints_range'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function rooms_availability_constraints_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  if ($instance['widget']['type'] == 'rooms_availability_constraints_range') {
    $field_parents = $element['#field_parents'];
    $field_name = $element['#field_name'];
    $language = $element['#language'];

    $parents = array_merge($field_parents, array($field_name, $language, $delta));

    $element += array(
      '#type' => 'rooms_availability_range',
      '#default_value' => isset($items[$delta]) ? $items[$delta] : '',
    );

    if ($field['cardinality'] == FIELD_CARDINALITY_UNLIMITED) {
      $element['remove'] = array(
        '#delta' => $delta,
        '#name' => implode('_', $parents) . '_remove_button',
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#validate' => array(),
        '#submit' => array('rooms_availability_constraints_remove_submit'),
        '#limit_validation_errors' => array(),
        '#ajax' => array(
          'path' => 'rooms_availability_constraints/ajax',
          'effect' => 'fade',
        ),
        '#attributes' => array(
          'class' => array('rooms-availability-range--remove-button'),
        ),
        '#weight' => 100,
      );

      $element['#attached']['css'] = array(drupal_get_path('module', 'rooms_availability_constraints') . '/css/rooms_availability_constraints_range.css');
    }
    return $element;
  }
}

/**
 * Implements hook_field_validate().
 */
function rooms_availability_constraints_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    if (is_array($item)) {
      if (isset($item['group_conditions']['start_date']) && isset($item['group_conditions']['end_date'])) {
        if ($item['group_conditions']['start_date'] >= $item['group_conditions']['end_date']) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'rooms_availability_constraints_end_date',
            'message' => t('The end date must be greater than the start date.'),
          );
        }
      }

      $minimum_set = $maximum_set = FALSE;
      if (!empty($item['group_conditions']['minimum_stay_nights'])) {
        if (!((int) $item['group_conditions']['minimum_stay_nights'] == $item['group_conditions']['minimum_stay_nights'] &&
              (int) $item['group_conditions']['minimum_stay_nights'] > 0)) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'rooms_availability_constraints_minimum_stay',
            'message' => t('The minimum stay must be a positive integer or zero (0).'),
          );
        }
        else {
          $minimum_set = TRUE;
        }
      }

      if (!empty($item['group_conditions']['maximum_stay_nights'])) {
        if (!((int) $item['group_conditions']['maximum_stay_nights'] == $item['group_conditions']['maximum_stay_nights'] &&
              (int) $item['group_conditions']['maximum_stay_nights'] > 0)) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'rooms_availability_constraints_maximum_stay',
            'message' => t('The maximum stay must be a positive integer or zero (0).'),
          );
        }
        else {
          $maximum_set = TRUE;
        }
      }

      if ($maximum_set && $minimum_set && ($item['group_conditions']['minimum_stay_nights'] > $item['group_conditions']['maximum_stay_nights'])) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'rooms_availability_constraints_range',
          'message' => t("The minimum stay cannot be longer than maximum stay."),
        );
      }
    }
  }
}

/**
 * Submit callback to remove an item from the field UI multiple wrapper.
 *
 * When a remove button is submitted, we need to find the item that it
 * referenced and delete it. Since field UI has the deltas as a straight
 * unbroken array key, we have to renumber everything down. Since we do this
 * we *also* need to move all the deltas around in the $form_state['values'],
 * $form_state['input'], and $form_state['field'] so that user changed values
 * follow. This is a bit of a complicated process.
 */
function rooms_availability_constraints_remove_submit($form, &$form_state) {
  $button = $form_state['triggering_element'];
  $delta = $button['#delta'];

  // Where in the form we'll find the parent element.
  $address = array_slice($button['#array_parents'], 0, -2);

  // Go one level up in the form, to the widgets container.
  $parent_element = drupal_array_get_nested_value($form, $address);
  $field_name = $parent_element['#field_name'];
  $langcode = $parent_element['#language'];
  $parents = $parent_element['#field_parents'];

  $field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);

  // Go ahead and renumber everything from our delta to the last
  // item down one. This will overwrite the item being removed.
  for ($i = $delta; $i <= $field_state['items_count']; $i++) {
    $old_element_address = array_merge($address, array($i + 1));
    $new_element_address = array_merge($address, array($i));

    $moving_element = drupal_array_get_nested_value($form, $old_element_address);
    $moving_element_value = drupal_array_get_nested_value($form_state['values'], $old_element_address);
    $moving_element_input = drupal_array_get_nested_value($form_state['input'], $old_element_address);
    $moving_element_field = drupal_array_get_nested_value($form_state['field'], $old_element_address);

    // Tell the element where it's being moved to.
    $moving_element['#parents'] = $new_element_address;

    // Move the element around.
    form_set_value($moving_element, $moving_element_value, $form_state);
    drupal_array_set_nested_value($form_state['input'], $moving_element['#parents'], $moving_element_input);
    drupal_array_set_nested_value($form_state['field'], $moving_element['#parents'], $moving_element_field);
  }

  // Replace the deleted entity with an empty one. This helps to ensure that
  // trying to add a new entity won't ressurect a deleted entity from the
  // trash bin.
  $form_state[$form['#entity_type']]->rooms_constraints_range = array();

  // Then remove the last item. But we must not go negative.
  if ($field_state['items_count'] > 0) {
    $field_state['items_count']--;
  }

  // Fix the weights. Field UI lets the weights be in a range of
  // (-1 * item_count) to (item_count). This means that when we remove one,
  // the range shrinks; weights outside of that range then get set to
  // the first item in the select by the browser, floating them to the top.
  // We use a brute force method because we lost weights on both ends
  // and if the user has moved things around, we have to cascade because
  // if I have items weight weights 3 and 4, and I change 4 to 3 but leave
  // the 3, the order of the two 3s now is undefined and may not match what
  // the user had selected.
  $input = drupal_array_get_nested_value($form_state['input'], $address);
  // Sort by weight
  uasort($input, '_field_sort_items_helper');

  // Reweight everything in the correct order.
  $weight = -1 * $field_state['items_count'];
  foreach ($input as $key => $item) {
    if ($item) {
      $input[$key]['_weight'] = $weight++;
    }
  }

  drupal_array_set_nested_value($form_state['input'], $address, $input);
  field_form_set_state($parents, $field_name, $langcode, $form_state, $field_state);

  $form_state['rebuild'] = TRUE;
}
