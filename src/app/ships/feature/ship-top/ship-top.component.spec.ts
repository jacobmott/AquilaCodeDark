import { ComponentFixture, TestBed } from "@angular/core/testing";
import { ShipstopComponent } from "./ship-top.component";

describe("ShipstopComponent", () => {
  let component: ShipstopComponent;
  let fixture: ComponentFixture<ShipstopComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ShipstopComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(ShipstopComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
