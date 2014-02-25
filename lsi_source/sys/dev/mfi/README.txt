README for LSI mfi driver v4.54.00.xx
=====================================


WARNING:  DO NOT upgrade to this driver for TBOLT/INVADER or newer LSI
          MegaRaid controllers.  This Driver only supports Liberator 
          controllers and controllers released prior to Liberator.





6-01-2012: Using a new versioning string.  Changed this driver version
           to v4.408.00.01.


5-25-2012: More TBOLT/INVADER data structures are removed.  Leave AEN 
           drain during driver attach time.  But leave a NOTE in the 
           code suggesting for design reconsideration.  Named the code
           set as version v4.54.00.02 as the wrap up version for the
           mfi legacy project.


5-10-2012: 1st pre-alpha release to customer with an official version 
           v4.54.00.01 which was named as v6.00.00.01 previously.  
           It has majority TBOLT and INVADER functionalities removed.  
           It was tested with a primary storage and a sencondary storage
           running tw_wrc with 50 threads R/W for 5 days continuously.


History:   This driver supports LSI MegaRaid Liberator controllers and 
           the MegaRaid controllers released prior to Liberator.  

           WARNING:  Do not upgrade to this driver for TBOLT/INVADER 
                     or newer controllers.  

           The source code of this driver is derived from mfi v4.36a, 
           but with TBOLT/INVADER functionalities removal.  

           It was initially named as version v6.00.00.00.  But later, 
           it was changed to version v4.54.00.00.
