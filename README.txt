INTRODUCTION
------------
This module adds functionality to the Rooms package
(drupal.org/project/rooms) that allow users to define availability
constraints.

Availability constraints are defined by a start date / end date,
can be activated only if they start on a certain day of the week
and can defined the maximum allowed stay for the date range.

Some typical usage scenario are:

"As an accommodation owner, during the months of July and August
I would like to ensure that all bookings are at least of four days
long"

"If a booking starts on a Friday I would like it to ensure that bookings
will be at least 2 days long"

"Bookings should always start on Saturday and last 7 days"

"Bookings should never exceed a total duration of 4 days"

INSTALLATION
------------
Simply download and activate the module.

Rooms Availability constraints requires:

rooms_availability (>=1.4)
rooms_booking (>=1.4)

CONFIGURATION
-------------
After activating the module visit the rooms unit type configuration page for
the type of units you want to activate constraints.

You can choose to either activate constraints for all units of a certain type
or on a per-unit basis.

After activating the constraints functionality you can then define the different
constraints.

The options are:

Always: Should the constraint always be active.
Start Date / End Date: If the constraint is not always active what is the range of
dates that is active for.
Day of the week related constraint: For the range already defined should this
constraint be activated only if the booking request starts on a specific date
or should it enforce that a booking needs to start on a specific date.
Minimum stay: The minimum allowable booking duration.
Maximum stay: The maximum allowable booking duration.

