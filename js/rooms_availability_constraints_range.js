(function ($) {
  /**
   * Availability Constraint Range options widget JS operations.
   */
  Drupal.behaviors.rooms_availability_constraint_range_widget = {
    attach: function(context) {
      $('.rooms-availability-range--remove-button').click(function(e) {
        $(this).parent().find('input[type=text]').val('');
        $(this).parents('tr').hide();
        e.preventDefault();
      });
    }
  };
})(jQuery);
