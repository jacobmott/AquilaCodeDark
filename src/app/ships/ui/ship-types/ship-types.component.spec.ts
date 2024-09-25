import { ComponentFixture, TestBed } from "@angular/core/testing";
import { ShipTypesComponent } from "./ship-types.component";

describe("ShipTypesComponent", () => {
  let component: ShipTypesComponent;
  let fixture: ComponentFixture<ShipTypesComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ShipTypesComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(ShipTypesComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
