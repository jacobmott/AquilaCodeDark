export class AngleConverter {
  private static readonly RADIANS_TO_DEGREES = 180 / Math.PI;
  private static readonly DEGREES_TO_RADIANS = Math.PI / 180;

  // Isometric angle offset (in degrees)
  private static readonly ISO_ANGLE_OFFSET = 45;

  /**
   * Converts a Cartesian angle to an isometric angle.
   * @param cartesianAngle - The input angle in degrees (0 degrees points right in Cartesian)
   * @returns The corresponding isometric angle in degrees
   */
  static cartesianToIsometric(cartesianAngle: number): number {
    // Convert to radians
    const radians = cartesianAngle * AngleConverter.DEGREES_TO_RADIANS;

    // Apply isometric transformation
    const isoX = Math.cos(radians) - Math.sin(radians);
    const isoY = (Math.cos(radians) + Math.sin(radians)) / 2;

    // Calculate the new angle
    let isoAngle = Math.atan2(isoY, isoX) * AngleConverter.RADIANS_TO_DEGREES;

    // Adjust the angle
    isoAngle += AngleConverter.ISO_ANGLE_OFFSET;

    // Normalize to 0-360 range
    return (isoAngle + 360) % 360;
  }

  /**
   * Converts an isometric angle to a Cartesian angle.
   * @param isometricAngle - The input angle in degrees (0 degrees points top-right in isometric)
   * @returns The corresponding Cartesian angle in degrees
   */
  static isometricToCartesian(isometricAngle: number): number {
    // Adjust for isometric offset
    const adjustedAngle = isometricAngle - AngleConverter.ISO_ANGLE_OFFSET;

    // Convert to radians
    const radians = adjustedAngle * AngleConverter.DEGREES_TO_RADIANS;

    // Apply inverse isometric transformation
    const cartX = Math.cos(radians) + Math.sin(radians) / 2;
    const cartY = Math.sin(radians) - Math.cos(radians) / 2;

    // Calculate the Cartesian angle
    let cartesianAngle =
      Math.atan2(cartY, cartX) * AngleConverter.RADIANS_TO_DEGREES;

    // Normalize to 0-360 range
    return (cartesianAngle + 360) % 360;
  }
}
