#include <kamek.h>


/* **************************************************************** */
/* *************** Bugged rotation-controlled actors ************** */
/* **************************************************************** */

// Fix actors that don't use the rotation controller's Starting Rotation value correctly.

// Some context: most rotation-controlled things inherit from daRotObjsBase_c and use its correctly-written
// functions for reading the rotation controller's info.
// But some handle it all manually themselves instead, and several don't read the starting-rotation value properly.

// All these actors have an "initial angle" field, representing their placement angle from the controller,
// calculated with atan2 upon load. The mistake they make is trying to also incorporate the controller's
// starting-rotation value into that, which is wrong because that's already included in the controller's rotation
// values themselves.

// daEnLiftRotHalf_c subtracts the value away, which results in that actor effectively ignoring starting-rotation.
// All the others add it instead, causing the starting-rotation to double.

// -0x4000 is used below as a replacement for instructions that read the rot. controller's starting-rotation value,
// because it's the difference between "up" (the default angle) and "right" (0 radians).
// This constant shows up frequently in Nintendo's own rotation-control code, too.

// daEnLiftRotHalf_c (EN_LIFT_ROTATION_HALF: profile 481, sprite 107)
kmWrite32(0x80a5d980, 0x38a0c000);  // li r5, -0x4000

// daEnObjRotationBlock_c (EN_ROTATION_BLOCK: profile 529, sprite 252)
kmWrite32(0x80a7b558, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x80a7b570, 0x3860c000);  // li r3, -0x4000

// daEnCoinAngle_c (EN_COIN_ANGLE: profile 530, sprite 253)
kmWrite32(0x809e55f4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809e560c, 0x3860c000);  // li r3, -0x4000

// daEnBlockAngle_c (EN_BLOCK_HATENA_ANGLE: profile 532, sprite 255 & EN_BLOCK_RENGA_ANGLE: profile 533, sprite 256)
kmWrite32(0x809c15c4, 0x3860c000);  // li r3, -0x4000
kmWrite32(0x809c15dc, 0x3860c000);  // li r3, -0x4000
