import { ComponentFixture, TestBed } from "@angular/core/testing";
import { SidebarAquilaCodeComponent } from "./sidebar-aquila-code.component";

describe("SidebarAquilaCodeComponent", () => {
  let component: SidebarAquilaCodeComponent;
  let fixture: ComponentFixture<SidebarAquilaCodeComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [SidebarAquilaCodeComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(SidebarAquilaCodeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
