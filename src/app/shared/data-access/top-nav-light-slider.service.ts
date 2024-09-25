import { Injectable } from "@angular/core";
import { Observable, BehaviorSubject } from "rxjs";

@Injectable({
  providedIn: "root",
})
export class TopNavLightSliderService {
  sharedCurrentXPositionBS: BehaviorSubject<number>;
  sharedCurrentXPosition: number;
  constructor() {
    this.sharedCurrentXPosition = 0;
    this.sharedCurrentXPositionBS = new BehaviorSubject<number>(0);
  }

  public getSharedCurrentXPositionBS(): Observable<number> {
    return this.sharedCurrentXPositionBS.asObservable();
  }

  public setSharedCurrentXPositionBS(position: number) {
    this.sharedCurrentXPositionBS.next(position);
  }

  public getSharedCurrentXPosition(): number {
    return this.sharedCurrentXPosition;
  }

  public setSharedCurrentXPosition(position: number) {
    this.sharedCurrentXPosition = position;
  }
}
