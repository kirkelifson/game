$cdmaterials "models\weapons"

$modelname	"weapons/v_momentum_gun.mdl"

$scale 1

$keyvalues { "prop_data" { "base" "metal.medium" } }

$model v_momentum_gun "v_momentum_gun.dmx"

$sequence "idle1" "anims/ACT_VM_IDLE" ACT_VM_IDLE 1 fps 15

$sequence "attack1" "v_momentum_gun" ACT_VM_PRIMARYATTACK 1 loop fps 30

$collisionmodel "v_momentum_gun.dmx"
{
	$mass 3.0 // Mass in kilograms
	$concave
}

//$attachment 1 "ValveBiped.1" 0 0 0 rotate 0 0 0

$attachment ValveBiped.Bip01_R_Hand "ValveBiped.Bip01_R_Hand" 0 0 0 rotate 0 0 0

$bonemerge ValveBiped.Bip01_R_Hand

$origin 125 25 2000