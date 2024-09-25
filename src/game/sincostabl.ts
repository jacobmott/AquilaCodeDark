export class SinCosTable {
  private sinTable: number[];
  private cosTable: number[];
  private resolution: number;

  constructor(resolution: number = 360) {
    this.resolution = resolution;
    this.sinTable = new Array(resolution);
    this.cosTable = new Array(resolution);
    this.generateTables();
  }

  private generateTables(): void {
    for (let i = 0; i < this.resolution; i++) {
      // const angle = (2 * Math.PI * i) / this.resolution;
      const radians = this.convertDegreesToRadians(i);

      const result = Math.sin(radians);
      const result2 = result.toFixed(4);
      const result3 = parseFloat(result2);

      //Flip the Y Axis
      if (i >= 0 && i <= 180) {
        this.sinTable[i] = -result3;
      } else {
        this.sinTable[i] = Math.abs(result3);
      }

      const result4 = Math.cos(radians);
      const result5 = result4.toFixed(4);
      const result6 = parseFloat(result5);

      this.cosTable[i] = result6;
    }
  }

  public getSin(angle: number): number {
    // const index = Math.round((angle * this.resolution) / 360) % this.resolution;
    return this.sinTable[angle];
  }

  public getCos(angle: number): number {
    // const index = Math.round((angle * this.resolution) / 360) % this.resolution;
    return this.cosTable[angle];
  }

  public printTables(): void {
    console.log("Sine Table:");
    console.log(this.sinTable);
    console.log("Cosine Table:");
    console.log(this.cosTable);
  }

  convertDegreesToRadians(degrees: number) {
    return degrees * (Math.PI / 180);
  }
}
